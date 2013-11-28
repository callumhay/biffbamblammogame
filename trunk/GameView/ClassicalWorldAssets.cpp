/**
 * ClassicalWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "ClassicalWorldAssets.h"
#include "Skybox.h"

#include "../ResourceManager.h"

ClassicalWorldAssets::ClassicalWorldAssets(GameAssets* assets) :
GameWorldAssets(assets, new Skybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->CLASSICAL_BLOCK_MESH)), 
fireColourFader(ColourRGBA(1.0f, 0.9f, 0.0f, 1.0f), ColourRGBA(0.3f, 0.10f, 0.0f, 0.2f)),
fireParticleScaler(1.0f, 0.025f), fireAccel1(Vector3D(1,1,1)), fireAccel2(Vector3D(1,1,1)) {

    // Initialize textures...
    assert(this->cloudTextures.empty());
    this->cloudTextures.reserve(3);
    Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD1, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);
    temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD2, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);
    temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD3, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);


    // Change the colours for the background mesh from the default to something a bit more washed out
    // (i.e., more classical-architecture-like)
    std::vector<Colour> colours;
    colours.reserve(8);
    colours.push_back(Colour(0.4375f, 0.5f, 0.5647f));             // slate greyish-blue
    colours.push_back(Colour(0.2745098f, 0.5098039f, 0.70588f));   // steel blue
    colours.push_back(Colour(0.51372549f, 0.4352941f, 1.0f));      // slate purple
    colours.push_back(Colour(0.8588235f, 0.439215686f, 0.57647f)); // pale violet
    colours.push_back(Colour(0.3725f, 0.6196078f, 0.62745098f));   // cadet (olive-) blue
    colours.push_back(Colour(0.4f, 0.8039215f, 0.666667f));		   // deep aquamarine
    colours.push_back(Colour(0.737f, 0.933f, 0.4078f));            // olive
    this->UpdateColourChangeList(colours);

    this->InitializeEmitters();
}

ClassicalWorldAssets::~ClassicalWorldAssets() {
    bool success = false;

    for (std::vector<Texture2D*>::iterator iter = this->cloudTextures.begin();
        iter != this->cloudTextures.end(); ++iter) {

            success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
            assert(success);	
    }
    this->cloudTextures.clear();

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
    static const float Z_FLAME_POS  = -11.25f;

    static const float FLAME_FREQ = 0.25f;
    static const float FLAME_SCALE = 0.33f;

	this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireEffect.SetScale(FLAME_SCALE);
	this->fireEffect.SetFrequency(FLAME_FREQ);
	this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect.SetTexture(this->cloudTextures[0]);

    bool result = false;

	fireEmitter1.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	fireEmitter1.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	fireEmitter1.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	fireEmitter1.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	fireEmitter1.SetEmitAngleInDegrees(5);
	fireEmitter1.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireEmitter1.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	fireEmitter1.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	fireEmitter1.SetEmitPosition(Point3D(X_FLAME1_POS, Y_FLAME_POS_WITH_OFFSET, Z_FLAME_POS));
    fireEmitter1.SetEmitDirection(Vector3D(0,1,0));
	fireEmitter1.AddEffector(&this->fireColourFader);
	fireEmitter1.AddEffector(&this->fireParticleScaler);
	fireEmitter1.AddEffector(&this->fireAccel1);
	result = fireEmitter1.SetRandomTextureEffectParticles(NUM_FIRE_PARTICLES, &this->fireEffect, this->cloudTextures);
	assert(result);

	fireEmitter2.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	fireEmitter2.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	fireEmitter2.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	fireEmitter2.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	fireEmitter2.SetEmitAngleInDegrees(5);
	fireEmitter2.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	fireEmitter2.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	fireEmitter2.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	fireEmitter2.SetEmitPosition(Point3D(X_FLAME2_POS, Y_FLAME_POS_WITH_OFFSET, Z_FLAME_POS));
    fireEmitter2.SetEmitDirection(Vector3D(0,1,0));
	fireEmitter2.AddEffector(&this->fireColourFader);
	fireEmitter2.AddEffector(&this->fireParticleScaler);
	fireEmitter2.AddEffector(&this->fireAccel2);
	result = fireEmitter2.SetRandomTextureEffectParticles(NUM_FIRE_PARTICLES, &this->fireEffect, this->cloudTextures);
	assert(result);

    UNUSED_VARIABLE(result);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (int i = 0; i < 60; i++) {
		this->fireEmitter1.Tick(0.5);
		this->fireEmitter2.Tick(0.5);
	}
}