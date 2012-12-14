/**
 * ClassicalWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ClassicalWorldAssets.h"

#include "../ResourceManager.h"

// TODO: Get rid of this
#include "DecoSkybox.h"

ClassicalWorldAssets::ClassicalWorldAssets() :
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BLOCK_MESH)), cloudTex(NULL),
fireColourFader(ColourRGBA(1.0f, 0.9f, 0.0f, 1.0f), ColourRGBA(0.3f, 0.10f, 0.0f, 0.2f)),
fireParticleScaler(1.0f, 0.025f), fireAccel1(Vector3D(1,1,1)), fireAccel2(Vector3D(1,1,1))

//smokeParticleScaler(1.0f, 1.75f),
//smokeRotatorCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::CLOCKWISE),
//smokeRotatorCCW(Randomizer::GetInstance()->RandomUnsignedInt() % 360, 0.25f, ESPParticleRotateEffector::COUNTER_CLOCKWISE)
{

    // Change the colours for the background mesh from the default to something a bit more washed out ... more classical architectury
    std::vector<Colour> colours;
    colours.reserve(8);
    colours.push_back(Colour(0.4375f, 0.5f, 0.5647f));             // slate greyish-blue
    colours.push_back(Colour(0.2745098f, 0.5098039f, 0.70588f));   // steel blue
    colours.push_back(Colour(0.51372549f, 0.4352941f, 1.0f));      // slate purple
    colours.push_back(Colour(0.8588235f, 0.439215686f, 0.57647f)); // pale violet
    colours.push_back(Colour(0.75f, 0.75f, 0.75f));                // silver
    colours.push_back(Colour(0.3725f, 0.6196078f, 0.62745098f));   // cadet (olive-) blue
    colours.push_back(Colour(0.4f, 0.8039215f, 0.666667f));		   // deep aquamarine
    colours.push_back(Colour(0.5f, 0.5f, 0.5f));                   // grey
    this->UpdateColourChangeList(colours);

    // Initialize textures...
	this->cloudTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CLOUD, Texture::Trilinear));
	assert(this->cloudTex != NULL);

    /*
	if (this->smokeTextures.empty()) {
		this->smokeTextures.reserve(6);
		Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);
		temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
		assert(temp != NULL);
		this->smokeTextures.push_back(temp);	
	}
    */

    this->InitializeEmitters();
}

ClassicalWorldAssets::~ClassicalWorldAssets() {
    bool success = false;

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->cloudTex);
    assert(success);

    /*
	for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
		iter != this->smokeTextures.end(); ++iter) {
		
		success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
		assert(success);	
	}
	this->smokeTextures.clear();

    for (std::vector<ESPPointEmitter*>::iterator iter = this->smokeEmitters1.begin();
         iter != this->smokeEmitters1.end(); ++iter) {
        ESPPointEmitter* currEmitter = *iter;
        delete currEmitter;
        currEmitter = NULL;
    }
    this->smokeEmitters1.clear();
    */

    UNUSED_VARIABLE(success);
}

void ClassicalWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                               const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void ClassicalWorldAssets::InitializeEmitters() {
    assert(this->cloudTex != NULL);

    static const float FIRE_SIZE_MIN = 2.6f;
    static const float FIRE_SIZE_MAX = 3.5f;
    static const float FIRE_LIFE_IN_SECS_MIN = 1.6f;
    static const float FIRE_LIFE_IN_SECS_MAX = 1.8f;
    static const float FIRE_SPAWN_DELTA_MIN = 0.025f;
    static const float FIRE_SPAWN_DELTA_MAX = 0.045f;
    static const float OFF_CENTER_AMT = 0.15f;
    static const int NUM_FIRE_PARTICLES = static_cast<int>(FIRE_LIFE_IN_SECS_MAX / FIRE_SPAWN_DELTA_MIN);
    static const float FIRE_SPEED_MIN = 4.0f;
    static const float FIRE_SPEED_MAX = 9.0f;

    static const float X_FLAME1_POS = 8.38f;
    static const float X_FLAME2_POS = -8.38f;
    static const float Y_FLAME_POS = 6.24f;
    static const float Y_FLAME_POS_WITH_OFFSET = Y_FLAME_POS - 0.75f;
    static const float Z_FLAME_POS  = -9.25f;

    static const float FLAME_FREQ = 0.25f;
    static const float FLAME_SCALE = 0.33f;

	this->fireEffect1.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect1.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireEffect1.SetScale(FLAME_SCALE);
	this->fireEffect1.SetFrequency(FLAME_FREQ);
	this->fireEffect1.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect1.SetMaskTexture(this->cloudTex);

	this->fireEffect2.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect2.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireEffect2.SetScale(FLAME_SCALE);
	this->fireEffect2.SetFrequency(FLAME_FREQ);
	this->fireEffect2.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect2.SetMaskTexture(this->cloudTex);

    bool result = false;

	fireEmitter1.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	fireEmitter1.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	fireEmitter1.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	fireEmitter1.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	fireEmitter1.SetEmitAngleInDegrees(5);
	fireEmitter1.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireEmitter1.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	fireEmitter1.SetParticleAlignment(ESP::ScreenAligned);
	fireEmitter1.SetEmitPosition(Point3D(X_FLAME1_POS, Y_FLAME_POS_WITH_OFFSET, Z_FLAME_POS));
    fireEmitter1.SetEmitDirection(Vector3D(0,1,0));
	fireEmitter1.AddEffector(&this->fireColourFader);
	fireEmitter1.AddEffector(&this->fireParticleScaler);
	fireEmitter1.AddEffector(&this->fireAccel1);
	result = fireEmitter1.SetParticles(NUM_FIRE_PARTICLES, &this->fireEffect1);
	assert(result);

	fireEmitter2.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	fireEmitter2.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	fireEmitter2.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	fireEmitter2.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	fireEmitter2.SetEmitAngleInDegrees(5);
	fireEmitter2.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireEmitter2.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	fireEmitter2.SetParticleAlignment(ESP::ScreenAligned);
	fireEmitter2.SetEmitPosition(Point3D(X_FLAME2_POS, Y_FLAME_POS_WITH_OFFSET, Z_FLAME_POS));
    fireEmitter2.SetEmitDirection(Vector3D(0,1,0));
	fireEmitter2.AddEffector(&this->fireColourFader);
	fireEmitter2.AddEffector(&this->fireParticleScaler);
	fireEmitter2.AddEffector(&this->fireAccel2);
	result = fireEmitter2.SetParticles(NUM_FIRE_PARTICLES, &this->fireEffect2);
	assert(result);

    /*
    static const float SMOKE_SPAWN_DELTA = FIRE_SPAWN_DELTA * static_cast<float>(this->smokeTextures.size());
    static const float SMOKE_LIFE_MIN = FIRE_LIFE_IN_SECS_MAX;
    static const float SMOKE_LIFE_MAX = 1.1f * FIRE_LIFE_IN_SECS_MAX;
    static const int NUM_SMOKE_PARTICLES_PER_EMITTER = static_cast<int>(SMOKE_LIFE_MAX / SMOKE_SPAWN_DELTA);
    this->smokeEmitters1.reserve(this->smokeTextures.size());

    for (int i = 0; i < static_cast<int>(this->smokeTextures.size()); i++) {
        
        ESPPointEmitter* smokeEmitter1 = new ESPPointEmitter();
	    smokeEmitter1->SetSpawnDelta(ESPInterval(SMOKE_SPAWN_DELTA));
	    smokeEmitter1->SetInitialSpd(ESPInterval(FIRE_SPEED_MAX, 1.25f * FIRE_SPEED_MAX));
	    smokeEmitter1->SetParticleLife(ESPInterval(SMOKE_LIFE_MIN, SMOKE_LIFE_MAX));
	    smokeEmitter1->SetParticleSize(ESPInterval(0.4f * FIRE_SIZE_MIN, 0.8f * FIRE_SIZE_MIN));
	    smokeEmitter1->SetEmitAngleInDegrees(10);
	    smokeEmitter1->SetEmitDirection(Vector3D(0, 1, 0));
	    smokeEmitter1->SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0.0f), ESPInterval(0.0f));
	    smokeEmitter1->SetParticleAlignment(ESP::ScreenAligned);
	    smokeEmitter1->SetEmitPosition(Point3D(X_FLAME1_POS, Y_FLAME_POS, Z_FLAME_POS));
	    smokeEmitter1->AddEffector(&this->fireColourFader);
        smokeEmitter1->AddEffector(&this->smokeParticleScaler);
        if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
            smokeEmitter1->AddEffector(&this->smokeRotatorCW);
        }
        else {
            smokeEmitter1->AddEffector(&this->smokeRotatorCCW);
        }
	    result = smokeEmitter1->SetParticles(NUM_SMOKE_PARTICLES_PER_EMITTER, this->smokeTextures[i]);
	    assert(result);
        
        smokeEmitter1->SimulateTicking(i / static_cast<float>(this->smokeTextures.size()) * SMOKE_LIFE_MIN);

        this->smokeEmitters1.push_back(smokeEmitter1);
    }
    */

    UNUSED_VARIABLE(result);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (int i = 0; i < 60; i++) {
		this->fireEmitter1.Tick(0.5);
		this->fireEmitter2.Tick(0.5);
	}
}