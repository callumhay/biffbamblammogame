/**
 * MineMeshManager.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MineMeshManager.h"
#include "GameViewConstants.h"

#include "../GameModel/PaddleMineProjectile.h"
#include "../GameModel/PlayerPaddle.h"

MineMeshManager::MineMeshManager() : mineMesh(NULL), trailTexture(NULL), pulseTexture(NULL),
timeSinceLastMineLaunch(0.0) {
    
    this->mineMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->MINE_MESH);
    assert(this->mineMesh != NULL);

    this->trailTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->trailTexture != NULL);

    this->pulseTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_OUTLINED_HOOP, Texture::Trilinear));
	assert(this->pulseTexture != NULL);

    this->triggeredTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->triggeredTexture != NULL);

    this->invisibleEffect.SetWarpAmountParam(50.0f);
    this->invisibleEffect.SetIndexOfRefraction(1.33f);
}

MineMeshManager::~MineMeshManager() {
    bool success = false;
    
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->mineMesh);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->trailTexture);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->pulseTexture);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->triggeredTexture);
	assert(success);

    UNUSED_VARIABLE(success);


    // Clean up the left-over mines...
    this->ClearMines();
}


void MineMeshManager::Draw(double dT, const Camera& camera,
                           const BasicPointLight& keyLight, const BasicPointLight& fillLight,
                           const BasicPointLight& ballLight, const Texture2D* sceneTex) {

    this->invisibleEffect.SetFBOTexture(sceneTex);

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (MineInstanceMapConstIter iter = this->mineInstanceMap.begin(); iter != this->mineInstanceMap.end(); ++iter) {
        MineInstance* mineInstance = iter->second;
        mineInstance->Draw(dT, camera, keyLight, fillLight, ballLight);
    }
    glPopAttrib();
}

void MineMeshManager::DrawLoadingMine(double dT, const PlayerPaddle& paddle, const Camera& camera,
                                      const BasicPointLight& keyLight, const BasicPointLight& fillLight,
                                      const BasicPointLight& ballLight, const Texture2D* sceneTex) {

    assert(paddle.HasPaddleType(PlayerPaddle::MineLauncherPaddle));

    CgFxEffectBase* replacementEffect = NULL;
    if (paddle.HasSpecialStatus(PlayerPaddle::InvisibleMineStatus)) {
        this->invisibleEffect.SetFBOTexture(sceneTex);
        replacementEffect = &this->invisibleEffect; 
    }

    // Draw the loaded/loading mine in the attachment on the paddle (if there is one)
    float mineStartHeight  = paddle.GetHalfHeight();
    float mineFinishHeight = paddle.GetMineProjectileStartingHeightRelativeToPaddle();
    float currMineHeight = NumberFuncs::LerpOverTime<float>(0.0, PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY,
        mineStartHeight, mineFinishHeight, std::min<double>(this->timeSinceLastMineLaunch, PlayerPaddle::PADDLE_MINE_LAUNCH_DELAY));

    glPushMatrix();
    glTranslatef(0.0f, currMineHeight, 0.0f);
    this->mineMesh->Draw(camera, replacementEffect, keyLight, fillLight, ballLight);
    glPopMatrix();

    this->timeSinceLastMineLaunch += dT;
}

void MineMeshManager::ClearMines() {
    for (MineInstanceMapIter iter = this->mineInstanceMap.begin(); iter != this->mineInstanceMap.end(); ++iter) {
        delete iter->second;
    }
    this->mineInstanceMap.clear();
}

#define TRAIL_EMITTER_DELTA_TIME 0.005f

MineMeshManager::MineInstance::MineInstance(MineMeshManager* manager, const MineProjectile* mine,
                                            Texture2D* trailTexture, Texture2D* pulseTexture,
                                            Texture2D* triggeredTexture) : manager(manager),
mine(mine), trailFader(Colour(1.0f,1.0f,1.0f), 0.6f, 0), pulseFader(ColourRGBA(0,1,0,1), ColourRGBA(1,0,0,0.1f)),
particleShrinkToNothing(1, 0), pulseGrower(1.0f, 4.0f), fastPulser(0,0) {

    assert(manager != NULL);
    assert(mine != NULL);

    bool result = true;

	this->trailEmitter.SetSpawnDelta(ESPInterval(TRAIL_EMITTER_DELTA_TIME));
	this->trailEmitter.SetInitialSpd(ESPInterval(0.0f));
	this->trailEmitter.SetParticleLife(ESPInterval(0.35f));
	this->trailEmitter.SetParticleSize(ESPInterval(1.3f), ESPInterval(1.3f));
	this->trailEmitter.SetEmitAngleInDegrees(0);
	this->trailEmitter.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->trailEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->trailEmitter.SetEmitPosition(Point3D(0, 0, 0));
	this->trailEmitter.AddEffector(&this->trailFader);
	this->trailEmitter.AddEffector(&this->particleShrinkToNothing);
	result = this->trailEmitter.SetParticles(35, trailTexture);
	assert(result);

	this->armedPulseEmitter.SetSpawnDelta(ESPInterval(0.25f));
	this->armedPulseEmitter.SetInitialSpd(ESPInterval(0));
	this->armedPulseEmitter.SetParticleLife(ESPInterval(1.0f));
	this->armedPulseEmitter.SetParticleSize(ESPInterval(1.5f));
	this->armedPulseEmitter.SetEmitAngleInDegrees(0);
	this->armedPulseEmitter.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->armedPulseEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->armedPulseEmitter.SetEmitPosition(Point3D(0, 0, 0));
	this->armedPulseEmitter.AddEffector(&this->pulseFader);
    this->armedPulseEmitter.AddEffector(&this->pulseGrower);
	result = this->armedPulseEmitter.SetParticles(4, pulseTexture);
	assert(result);
	
	this->fastPulseSettings.pulseGrowthScale = 1.0f;
	this->fastPulseSettings.pulseRate = 1.0f;
	this->fastPulser = ESPParticleScaleEffector(this->fastPulseSettings);

	this->triggeredPulseEmitter.SetSpawnDelta(ESPInterval(-1));
	this->triggeredPulseEmitter.SetInitialSpd(ESPInterval(0));
	this->triggeredPulseEmitter.SetParticleLife(ESPInterval(-1));
    this->triggeredPulseEmitter.SetParticleSize(ESPInterval(1.5f * this->mine->GetWidth()));
	this->triggeredPulseEmitter.SetEmitAngleInDegrees(0);
	this->triggeredPulseEmitter.SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->triggeredPulseEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->triggeredPulseEmitter.SetEmitPosition(Point3D(0, 0, 0));
	this->triggeredPulseEmitter.SetParticleColour(ESPInterval(1), ESPInterval(0), ESPInterval(0), ESPInterval(0.75f));
	this->triggeredPulseEmitter.AddEffector(&this->fastPulser);
	result = this->triggeredPulseEmitter.SetParticles(1, triggeredTexture);
	assert(result);
}

MineMeshManager::MineInstance::~MineInstance() {
}

void MineMeshManager::MineInstance::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                                         const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    const Point2D& position = this->mine->GetPosition();

    if (!this->mine->GetIsInvisible()) {

        // Draw the trail emitter for the mine
        if (this->mine->IsLoadedInCannonBlock() ||
            this->mine->GetVelocityMagnitude() <= EPSILON && Vector2D::Dot(this->mine->GetVelocityDirection(), Vector2D(0, 1)) <= 0) {
            this->trailEmitter.SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
        }
        else {
            this->trailEmitter.SetSpawnDelta(ESPInterval(TRAIL_EMITTER_DELTA_TIME));
        }
     
        this->trailEmitter.SetParticleSpawnSize(ESPInterval(mine->GetWidth()));
        this->trailEmitter.SetEmitPosition(Point3D(position[0], position[1], 0.0f));
        this->trailEmitter.Tick(dT);
        this->trailEmitter.Draw(camera);
    }
    
    // Don't draw the mine while it's loaded in a cannon
    if (this->mine->IsLoadedInCannonBlock()) {
        return;
    }

    glPushMatrix();
    glTranslatef(position[0], position[1], this->mine->GetHalfWidth());

    glPushAttrib(GL_DEPTH_BUFFER_BIT);

    // Special cases for dealing with invisible mines...
    CgFxEffectBase* replacementEffect = NULL;
    ESPPointEmitter* currTriggeredPulseEmitter = &this->triggeredPulseEmitter;
    ESPPointEmitter* currArmedPulseEmitter     = &this->armedPulseEmitter;

    if (this->mine->GetIsInvisible()) {
        replacementEffect = &this->manager->invisibleEffect;
        // TODO: set invisible emitters ?
        glDepthMask(GL_FALSE);
    }
    else {

        if (this->mine->GetIsProximityAlerted()) {
            // The mine has been triggered - show the player that the mine is going to explode soon...

            // Make the mine's trigger aura pulse faster as the countdown time gets closer to zero
            float pulseRateAndGrowth = NumberFuncs::Lerp<float>(PaddleMineProjectile::MINE_MAX_COUNTDOWN_TIME,
                0.0f, 1.25f, 3.5f, this->mine->GetProximityCountdownInSeconds());
            this->fastPulseSettings.pulseGrowthScale = pulseRateAndGrowth;
            this->fastPulseSettings.pulseRate = pulseRateAndGrowth;
            this->fastPulser.ResetScaleEffect(this->fastPulseSettings);

            currTriggeredPulseEmitter->Tick(dT);
            currTriggeredPulseEmitter->Draw(camera);
        }
        else {
            // Draw any proximity pulse for the mine if it's armed
            if (this->mine->GetIsArmed()) {
                currArmedPulseEmitter->SetParticleSpawnSize(ESPInterval(mine->GetWidth()));
                currArmedPulseEmitter->Tick(dT);
                currArmedPulseEmitter->Draw(camera);
            }
            else {
                currArmedPulseEmitter->Reset();
            }
        }
    }

    float currRotation = this->mine->GetCurrentRotation();
    glRotatef(currRotation, 1, 0, 0);
    glRotatef(0.75f * currRotation, 0, 1, 0);
    
    float visualScale = this->mine->GetVisualScaleFactor();
    glScalef(visualScale, visualScale, visualScale);

    // Draw the actual mine
    this->manager->mineMesh->Draw(camera, replacementEffect, keyLight, fillLight, ballLight);
    

    glPopAttrib();
    glPopMatrix();
}
