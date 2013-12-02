/**
 * GothicRomanticWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GothicRomanticWorldAssets.h"
#include "Skybox.h"

#include "../GameModel/GameModel.h"
#include "../ResourceManager.h"

const float GothicRomanticWorldAssets::MOON_CLOUD_ALPHA         = 0.4f;
const float GothicRomanticWorldAssets::LESS_VISIBLE_CLOUD_ALPHA = 0.2f;

GothicRomanticWorldAssets::GothicRomanticWorldAssets(GameAssets* assets) : 
GameWorldAssets(assets, new Skybox(),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BACKGROUND_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_PADDLE_MESH),
    ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->GOTHIC_ROMANTIC_BLOCK_MESH)),
fireColourFader(ColourRGBA(1.0f, 0.9f, 0.0f, 1.0f), ColourRGBA(0.3f, 0.10f, 0.0f, 0.2f)),
fireParticleScaler(1.0f, 0.025f), moonTex(NULL),
fireAccel1(Vector3D(1,1,1)), fireAccel2(Vector3D(1,1,1)),
fireAccel4(Vector3D(1,1,1)), fireAccel5(Vector3D(1,1,1)), fireAccel6(Vector3D(1,1,1)),
moonCloudFader(), cloudGrower(1.0f, 1.15f), moonPos(23.0f, 38.0f, -50.0f) {

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

    this->moonTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_MOON, Texture::Trilinear));
    assert(this->moonTex != NULL);

    this->skybox->SetFGStarMoveSpd(0.001f);

    /*
    // TODO... 
    // Change the colours for the background mesh from the default to something more gothic-architectury
    std::vector<Colour> colours;
    colours.reserve(?);

    this->UpdateColourChangeList(colours);
    */

    this->InitializeEmitters();
}

GothicRomanticWorldAssets::~GothicRomanticWorldAssets() {
    bool success = false;

    for (std::vector<Texture2D*>::iterator iter = this->cloudTextures.begin();
        iter != this->cloudTextures.end(); ++iter) {

            success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
            assert(success);	
    }
    this->cloudTextures.clear();

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->moonTex);
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

void GothicRomanticWorldAssets::DrawBackgroundPostOutlinePreEffects(const Camera& camera) {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(this->moonPos[0], this->moonPos[1], this->moonPos[2]);
    glScalef(15.0f, 15.0f, 1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, this->bgFadeAnim.GetInterpolantValue());
    this->moonTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->moonTex->UnbindTexture();
    glPopMatrix();

    this->lowerCloudEmitter.Draw(camera);
    this->moonCloudEmitter.Draw(camera);

    glPopAttrib();
}

void GothicRomanticWorldAssets::Tick(double dT, const GameModel& model) {
    GameWorldAssets::Tick(dT, model);

    this->ApplyRandomFireAccel(fireAccel1);
    this->ApplyRandomFireAccel(fireAccel2);
    this->ApplyRandomFireAccel(fireAccel4);
    this->ApplyRandomFireAccel(fireAccel5);
    this->ApplyRandomFireAccel(fireAccel6);

    // Tick the emitters
    float alpha = this->bgFadeAnim.GetInterpolantValue();

    this->fireEffect.SetAlphaMultiplier(alpha);
    this->fireEmitter1.Tick(dT);
    this->fireEmitter1.SetAliveParticleAlphaMax(alpha);
    this->fireEmitter2.Tick(dT);
    this->fireEmitter2.SetAliveParticleAlphaMax(alpha);

    this->fireEmitter4.Tick(dT);
    this->fireEmitter4.SetAliveParticleAlphaMax(alpha);
    this->fireEmitter5.Tick(dT);
    this->fireEmitter5.SetAliveParticleAlphaMax(alpha);
    this->fireEmitter6.Tick(dT);
    this->fireEmitter6.SetAliveParticleAlphaMax(alpha);

    this->lowerCloudEmitter.Tick(dT);
    this->lowerCloudEmitter.SetAliveParticleAlphaMax(LESS_VISIBLE_CLOUD_ALPHA*alpha);

    this->moonCloudEmitter.Tick(dT);
    this->moonCloudEmitter.SetAliveParticleAlphaMax(MOON_CLOUD_ALPHA*alpha);

    this->cloudEffect.SetLightPos(moonPos + model.GetCurrentLevelTranslation());
}

void GothicRomanticWorldAssets::InitializeEmitters() {
    static const float FLAME_FREQ = 0.25f;
    static const float FLAME_SCALE = 0.33f;

	this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
	this->fireEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
	this->fireEffect.SetScale(FLAME_SCALE);
	this->fireEffect.SetFrequency(FLAME_FREQ);
	this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
	this->fireEffect.SetTexture(this->cloudTextures[0]);

	this->BuildFrontDoorFireEmitter(Point3D(7.180f, 0.85f, -0.690f), fireEmitter1);
	fireEmitter1.AddEffector(&this->fireAccel1);

    this->BuildFrontDoorFireEmitter(Point3D(10.067f, 0.85f, -0.690f), fireEmitter2);
	fireEmitter2.AddEffector(&this->fireAccel2);

    this->BuildWindowWallFireEmitter(Point3D(-5.988f, 0.5f, -14.013), fireEmitter4);
	fireEmitter4.AddEffector(&this->fireAccel4);

	this->BuildWindowWallFireEmitter(Point3D(-3.438f, 0.5f, -14.013), fireEmitter5);
	fireEmitter5.AddEffector(&this->fireAccel5);

    this->BuildWindowWallFireEmitter(Point3D(-0.888f, 0.5f, -14.013), fireEmitter6);
	fireEmitter6.AddEffector(&this->fireAccel6);

	// Tick all the emitters for a bit to get them to look like they've been spawning for awhile
	for (int i = 0; i < 60; i++) {
		this->fireEmitter1.Tick(0.5);
		this->fireEmitter2.Tick(0.5);
		this->fireEmitter4.Tick(0.5);
		this->fireEmitter5.Tick(0.5);
		this->fireEmitter6.Tick(0.5);
	}

    static const float CLOUD_GREY_AMT = 0.25f;
    this->cloudEffect.SetTechnique(CgFxVolumetricEffect::CLOUDYSPRITE_TECHNIQUE_NAME);
    this->cloudEffect.SetColour(Colour(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT));
    this->cloudEffect.SetScale(0.25f);
    this->cloudEffect.SetFrequency(0.08f);
    this->cloudEffect.SetFlowDirection(Vector3D(-1, 0, 1));
    this->cloudEffect.SetTexture(this->cloudTextures[0]);
    this->cloudEffect.SetLightPos(this->moonPos);
    this->cloudEffect.SetAttenuation(0.035f);

    std::vector<ColourRGBA> cloudColours;
    cloudColours.reserve(6);
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, 0.0f));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, MOON_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, MOON_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, MOON_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, MOON_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, 0.0f));
    this->moonCloudFader.SetColours(cloudColours);

    cloudColours.clear();
    cloudColours.reserve(6);
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, 0.0f));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, LESS_VISIBLE_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, LESS_VISIBLE_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, LESS_VISIBLE_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, LESS_VISIBLE_CLOUD_ALPHA));
    cloudColours.push_back(ColourRGBA(CLOUD_GREY_AMT, CLOUD_GREY_AMT, CLOUD_GREY_AMT, 0.0f));
    this->lessVisibleCloudFader.SetColours(cloudColours);

    // Moon cloud emitter
    this->BuildCloudEmitter(Point3D(-70.0f, moonPos[1] - 4.0f, -45.0f), 
        Point3D(-50.0f, moonPos[1] + 5.0f, -42.0f), 1, this->moonCloudFader, this->moonCloudEmitter);
    // Lower clouds
    this->BuildCloudEmitter(Point3D(-70.0f, moonPos[1] - 38.0f, -49.0f), 
        Point3D(-50.0f, moonPos[1] - 9.0f, -46.0f), 1, this->lessVisibleCloudFader, this->lowerCloudEmitter);
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

void GothicRomanticWorldAssets::BuildCloudEmitter(const Point3D& min, const Point3D& max, int dir, 
                                                  ESPMultiColourEffector& colourEffector, ESPVolumeEmitter& emitter) {

    emitter.SetEmitVolume(min, max);
    emitter.SetSpawnDelta(ESPInterval(45.0f, 55.0f));
    emitter.SetInitialSpd(ESPInterval(0.6f, 0.85f));
    emitter.SetParticleLife(ESPInterval(190.0f, 210.0f));
    emitter.SetParticleSize(ESPInterval(60.0f, 100.0f), ESPInterval(15.0f, 22.0f));
    emitter.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    emitter.SetEmitDirection(Vector3D(dir*1,0,0));
    emitter.AddEffector(&colourEffector);
    emitter.AddEffector(&this->cloudGrower);
    emitter.SetRandomTextureEffectParticles(10, &this->cloudEffect, this->cloudTextures);

    for (int i = 0; i < 100; i++) {
        emitter.Tick(10.0);
    }
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
	emitter.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	emitter.SetEmitPosition(pos);
    emitter.SetEmitDirection(Vector3D(0,1,0));
	emitter.AddEffector(&this->fireColourFader);
	emitter.AddEffector(&this->fireParticleScaler);
	emitter.SetRandomTextureEffectParticles(NUM_FIRE_PARTICLES, &this->fireEffect, this->cloudTextures);
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
	emitter.SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	emitter.SetEmitPosition(pos);
    emitter.SetEmitDirection(Vector3D(0,1,0));
	emitter.AddEffector(&this->fireColourFader);
	emitter.AddEffector(&this->fireParticleScaler);
	emitter.SetParticles(NUM_FIRE_PARTICLES, &this->fireEffect);
}

void GothicRomanticWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    
    this->fireEmitter1.Draw(camera);
    this->fireEmitter2.Draw(camera);
    this->fireEmitter4.Draw(camera);
    this->fireEmitter5.Draw(camera);
    this->fireEmitter6.Draw(camera);
}
