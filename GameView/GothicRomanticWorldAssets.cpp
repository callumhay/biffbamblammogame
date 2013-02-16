/**
 * GothicRomanticWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GothicRomanticWorldAssets.h"

#include "../ResourceManager.h"

// TODO: Get rid of this
#include "DecoSkybox.h"

GothicRomanticWorldAssets::GothicRomanticWorldAssets() : 
GameWorldAssets(new DecoSkybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BLOCK_MESH)),
fireColourFader(ColourRGBA(1.0f, 0.9f, 0.0f, 1.0f), ColourRGBA(0.3f, 0.10f, 0.0f, 0.2f)),
fireParticleScaler(1.0f, 0.025f),
fireAccel1(Vector3D(1,1,1)), fireAccel2(Vector3D(1,1,1)),
fireAccel3(Vector3D(1,1,1)), fireAccel4(Vector3D(1,1,1)),
fireAccel5(Vector3D(1,1,1)), fireAccel6(Vector3D(1,1,1)){

    // Initialize textures...
	this->cloudTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CLOUD, Texture::Trilinear));
	assert(this->cloudTex != NULL);

    /*
    // TODO... 
    // Change the colours for the background mesh from the default to something a bit more washed out ... more classical architectury
    std::vector<Colour> colours;
    colours.reserve(?);

    colours.push_back(Colour(0.51372549f, 0.4352941f, 1.0f));      // slate purple
    colours.push_back(Colour(0.8588235f, 0.439215686f, 0.57647f)); // pale violet
    colours.push_back(Colour(0.737f, 0.933f, 0.4078f));            // olive



    this->UpdateColourChangeList(colours);
    */

    this->InitializeEmitters();
}

GothicRomanticWorldAssets::~GothicRomanticWorldAssets() {
    bool success = false;

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->cloudTex);
    assert(success);

    UNUSED_VARIABLE(success);
}

void GothicRomanticWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                                    const BasicPointLight& bgFillLight) {

    const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();

    // Draw the model with the current background colour
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void GothicRomanticWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);

    this->ApplyRandomFireAccel(fireAccel1);
    this->ApplyRandomFireAccel(fireAccel2);
    this->ApplyRandomFireAccel(fireAccel3);
    this->ApplyRandomFireAccel(fireAccel4);
    this->ApplyRandomFireAccel(fireAccel5);
    this->ApplyRandomFireAccel(fireAccel6);

    // Tick the emitters
    float alpha = this->bgFadeAnim.GetInterpolantValue();

    this->fireEffect.SetAlphaMultiplier(alpha);
    this->fireEmitter1.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter1.Tick(dT);
    this->fireEmitter2.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter2.Tick(dT);

    this->fireEmitter3.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter3.Tick(dT);
    this->fireEmitter4.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter4.Tick(dT);
    this->fireEmitter5.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter5.Tick(dT);
    this->fireEmitter6.SetParticleAlpha(ESPInterval(alpha));
    this->fireEmitter6.Tick(dT);
}

void GothicRomanticWorldAssets::InitializeEmitters() {
    assert(this->cloudTex != NULL);

    static const float FLAME_FREQ = 0.25f;
    static const float FLAME_SCALE = 0.33f;

	this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireEffect.SetScale(FLAME_SCALE);
	this->fireEffect.SetFrequency(FLAME_FREQ);
	this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect.SetMaskTexture(this->cloudTex);

	this->BuildFrontDoorFireEmitter(Point3D(8.180f, 0.85f, 0.310f), fireEmitter1);
	fireEmitter1.AddEffector(&this->fireAccel1);

    this->BuildFrontDoorFireEmitter(Point3D(11.067f, 0.85f, 0.310f), fireEmitter2);
	fireEmitter2.AddEffector(&this->fireAccel2);

	this->BuildWindowWallFireEmitter(Point3D(-7.55f, 0.5f, -13.013f), fireEmitter3);
	fireEmitter3.AddEffector(&this->fireAccel3);

    this->BuildWindowWallFireEmitter(Point3D(-4.988f, 0.5f, -13.013), fireEmitter4);
	fireEmitter4.AddEffector(&this->fireAccel4);

	this->BuildWindowWallFireEmitter(Point3D(-2.438f, 0.5f, -13.013), fireEmitter5);
	fireEmitter5.AddEffector(&this->fireAccel5);

    this->BuildWindowWallFireEmitter(Point3D(0.112f, 0.5f, -13.013), fireEmitter6);
	fireEmitter6.AddEffector(&this->fireAccel6);


	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (int i = 0; i < 60; i++) {
		this->fireEmitter1.Tick(0.5);
		this->fireEmitter2.Tick(0.5);
		this->fireEmitter3.Tick(0.5);
		this->fireEmitter4.Tick(0.5);
		this->fireEmitter5.Tick(0.5);
		this->fireEmitter6.Tick(0.5);
	}
}

void GothicRomanticWorldAssets::ApplyRandomFireAccel(ESPParticleAccelEffector& accelEffector) {
    static const float CONE_SIZE_IN_DEGREES = 5.0f;
    static const float ACCEL_MAG = 3.0f;
	
    // Rotate the up direction by some random amount and then affect the fire particle's velocities
	// by it, this gives the impression that the particles are flickering like fire
	double randomDegrees = Randomizer::GetInstance()->RandomNumNegOneToOne() * CONE_SIZE_IN_DEGREES;
	Vector2D accelVec = ACCEL_MAG * Vector2D::Rotate(static_cast<float>(randomDegrees), Vector2D(0, 1));
	accelEffector.SetAcceleration(Vector3D(accelVec, 0.0f));
}

void GothicRomanticWorldAssets::BuildFrontDoorFireEmitter(const Point3D& pos, ESPPointEmitter& emitter) {

    static const float FIRE_SIZE_MIN = 0.5f;
    static const float FIRE_SIZE_MAX = 1.0f;
    static const float FIRE_LIFE_IN_SECS_MIN = 0.6f;
    static const float FIRE_LIFE_IN_SECS_MAX = 1.3f;
    static const float FIRE_SPAWN_DELTA_MIN = 0.025f;
    static const float FIRE_SPAWN_DELTA_MAX = 0.045f;
    static const float OFF_CENTER_AMT = 0.05f;
    static const int NUM_FIRE_PARTICLES = static_cast<int>(FIRE_LIFE_IN_SECS_MAX / FIRE_SPAWN_DELTA_MIN);
    static const float FIRE_SPEED_MIN = 1.25f;
    static const float FIRE_SPEED_MAX = 3.0f;

	emitter.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	emitter.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	emitter.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	emitter.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	emitter.SetEmitAngleInDegrees(5);
	emitter.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	emitter.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	emitter.SetParticleAlignment(ESP::ScreenAligned);
	emitter.SetEmitPosition(pos);
    emitter.SetEmitDirection(Vector3D(0,1,0));
	emitter.AddEffector(&this->fireColourFader);
	emitter.AddEffector(&this->fireParticleScaler);
	emitter.SetParticles(NUM_FIRE_PARTICLES, &this->fireEffect);
}

void GothicRomanticWorldAssets::BuildWindowWallFireEmitter(const Point3D& pos, ESPPointEmitter& emitter) {
    static const float FIRE_SIZE_MIN = 0.5f;
    static const float FIRE_SIZE_MAX = 1.0f;
    static const float FIRE_LIFE_IN_SECS_MIN = 0.6f;
    static const float FIRE_LIFE_IN_SECS_MAX = 1.3f;
    static const float FIRE_SPAWN_DELTA_MIN = 0.025f;
    static const float FIRE_SPAWN_DELTA_MAX = 0.045f;
    static const float OFF_CENTER_AMT = 0.05f;
    static const int NUM_FIRE_PARTICLES = static_cast<int>(FIRE_LIFE_IN_SECS_MAX / FIRE_SPAWN_DELTA_MIN);
    static const float FIRE_SPEED_MIN = 1.0f;
    static const float FIRE_SPEED_MAX = 2.8f;

	emitter.SetSpawnDelta(ESPInterval(FIRE_SPAWN_DELTA_MIN, FIRE_SPAWN_DELTA_MAX));
	emitter.SetInitialSpd(ESPInterval(FIRE_SPEED_MIN, FIRE_SPEED_MAX));
	emitter.SetParticleLife(ESPInterval(FIRE_LIFE_IN_SECS_MIN, FIRE_LIFE_IN_SECS_MAX));
	emitter.SetParticleSize(ESPInterval(FIRE_SIZE_MIN, FIRE_SIZE_MAX));
	emitter.SetEmitAngleInDegrees(5);
	emitter.SetParticleRotation(ESPInterval(-180.0f, 180.0f));
	emitter.SetRadiusDeviationFromCenter(ESPInterval(0.0f, OFF_CENTER_AMT), ESPInterval(0), ESPInterval(0.0f, OFF_CENTER_AMT));
	emitter.SetParticleAlignment(ESP::ScreenAligned);
	emitter.SetEmitPosition(pos);
    emitter.SetEmitDirection(Vector3D(0,1,0));
	emitter.AddEffector(&this->fireColourFader);
	emitter.AddEffector(&this->fireParticleScaler);
	emitter.SetParticles(NUM_FIRE_PARTICLES, &this->fireEffect);
}