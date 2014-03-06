/**
 * RocketMesh.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "RocketMesh.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPPointEmitter.h"

#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/PaddleRemoteControlRocketProjectile.h"
#include "../GameModel/PlayerPaddle.h"

RocketMesh::RocketMesh() : 
paddleRocketMesh(NULL), paddleRemoteControlRocketMesh(NULL), turretRocketMesh(NULL), sparkTex(NULL),
rocketGlowEmitter(NULL), rocketThrustBurstEmitter(NULL), pulseEffector(0,0), particleFader(1, 0),
rocketThrustHyperBurnEmitter(NULL), cloudNormalTex(NULL),
hyperBurnColourFader(ColourRGBA(1.0f, 1.0f, 0.9f, 1.0f), ColourRGBA(0.8f, 0.8f, 0.1f, 0.0f)),
particleBurstColourEffector(ColourRGBA(1.0f, 1.0f, 0.1f, 1.0f), ColourRGBA(0.5f, 0.0f, 0.0f, 0.0f)),
particleLargeGrowth(1.0f, 2.2f),
loopRotateEffectorCW(180.0f, ESPParticleRotateEffector::CLOCKWISE),
loopRotateEffectorCCW(180.0f, ESPParticleRotateEffector::COUNTER_CLOCKWISE) {

	this->LoadMeshes();

    this->invisibleEffect.SetWarpAmountParam(50.0f);
    this->invisibleEffect.SetIndexOfRefraction(1.33f);

    this->cloudNormalTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_NORMAL_CLOUD, Texture::Trilinear));
    assert(this->cloudNormalTex != NULL);

    this->invisibleThrustEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_WITH_NOISE_TECHNIQUE_NAME);
    this->invisibleThrustEffect.SetWarpAmountParam(30.0f);
    this->invisibleThrustEffect.SetIndexOfRefraction(1.33f);
    this->invisibleThrustEffect.SetScale(0.02f);
    this->invisibleThrustEffect.SetFrequency(0.08f);
    this->invisibleThrustEffect.SetNormalTexture(this->cloudNormalTex);

	this->glowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Trilinear));
	assert(this->glowTex != NULL);
	
	ScaleEffect pulseSettings;
	pulseSettings.pulseGrowthScale = 1.8f;
	pulseSettings.pulseRate = 0.75f;
	this->pulseEffector = ESPParticleScaleEffector(pulseSettings);

	this->rocketGlowEmitter = new ESPPointEmitter();
	this->rocketGlowEmitter->SetSpawnDelta(ESPInterval(-1));
	this->rocketGlowEmitter->SetInitialSpd(ESPInterval(0));
	this->rocketGlowEmitter->SetParticleLife(ESPInterval(-1));
	this->rocketGlowEmitter->SetParticleSize(ESPInterval(1), ESPInterval(1));
	this->rocketGlowEmitter->SetEmitAngleInDegrees(0);
	this->rocketGlowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->rocketGlowEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
	this->rocketGlowEmitter->SetEmitPosition(Point3D(0, 0, 0));
	this->rocketGlowEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.70f));
	this->rocketGlowEmitter->AddEffector(&this->pulseEffector);
	this->rocketGlowEmitter->SetParticles(1, this->glowTex);

    if (this->smokeTextures.empty()) {
        this->smokeTextures.reserve(6);
        Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE1, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE2, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE3, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE4, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE5, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);
        temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
            GameViewConstants::GetInstance()->TEXTURE_SMOKE6, Texture::Trilinear));
        assert(temp != NULL);
        this->smokeTextures.push_back(temp);	
    }

    this->rocketThrustBurstEmitter = new ESPPointEmitter();
    this->rocketThrustBurstEmitter->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
    this->rocketThrustBurstEmitter->SetNumParticleLives(0); // Make initial lives zero so the emitter doesn't go off right as the rocket takes off
    this->rocketThrustBurstEmitter->SetInitialSpd(ESPInterval(
        0.9f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_BEFORE_THRUST,
        0.9f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_WITH_THRUST));
    this->rocketThrustBurstEmitter->SetParticleLife(ESPInterval(2.0f, 3.0f));
    this->rocketThrustBurstEmitter->SetEmitAngleInDegrees(45);
    this->rocketThrustBurstEmitter->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f),
        ESPInterval(0.0f), 
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f));
    this->rocketThrustBurstEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->rocketThrustBurstEmitter->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
    this->rocketThrustBurstEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->rocketThrustBurstEmitter->AddEffector(&this->particleBurstColourEffector);
    this->rocketThrustBurstEmitter->AddEffector(&this->particleLargeGrowth);
    this->rocketThrustBurstEmitter->SetRandomTextureParticles(20, this->smokeTextures);

    this->sparkTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_SPARKLE, Texture::Trilinear));
    assert(this->sparkTex != NULL);

    this->rocketThrustingSparksEmitter = new ESPPointEmitter();
    this->rocketThrustingSparksEmitter->SetSpawnDelta(ESPInterval(0.02f, 0.05f));
    this->rocketThrustingSparksEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES); // Make initial lives zero so the emitter doesn't go off right as the rocket takes off
    this->rocketThrustingSparksEmitter->SetInitialSpd(ESPInterval(
        0.5f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_BEFORE_THRUST,
        0.5f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_WITH_THRUST));
    this->rocketThrustingSparksEmitter->SetParticleLife(ESPInterval(0.5f, 1.0f));
    this->rocketThrustingSparksEmitter->SetEmitAngleInDegrees(45);
    this->rocketThrustingSparksEmitter->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f),
        ESPInterval(0.0f), 
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f));
    this->rocketThrustingSparksEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->rocketThrustingSparksEmitter->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
    this->rocketThrustingSparksEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->rocketThrustingSparksEmitter->AddEffector(&this->particleLargeGrowth);
    this->rocketThrustingSparksEmitter->SetParticles(50, this->sparkTex);

    // Fire effect used in various things - like explosions and such.
    this->fireEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
    this->fireEffect.SetColour(Colour(1.00f, 1.00f, 1.00f));
    this->fireEffect.SetScale(0.25f);
    this->fireEffect.SetFrequency(1.5f);
    this->fireEffect.SetFlowDirection(Vector3D(0, 0, 1));
    this->fireEffect.SetTexture(this->glowTex);

    this->rocketThrustHyperBurnEmitter = new ESPPointEmitter();
    this->rocketThrustHyperBurnEmitter->SetSpawnDelta(ESPInterval(0.01f));
    this->rocketThrustHyperBurnEmitter->SetParticleLife(ESPInterval(0.5f, 0.75f));
    this->rocketThrustHyperBurnEmitter->SetEmitAngleInDegrees(20);
    this->rocketThrustHyperBurnEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->rocketThrustHyperBurnEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->rocketThrustHyperBurnEmitter->AddEffector(&this->hyperBurnColourFader);
    this->rocketThrustHyperBurnEmitter->AddEffector(&this->particleLargeGrowth);
    this->rocketThrustHyperBurnEmitter->SetParticles(25, &this->fireEffect);

    this->invisibleRocketThrustEmitter = new ESPPointEmitter();
    this->invisibleRocketThrustEmitter->SetSpawnDelta(ESPInterval(ESPPointEmitter::ONLY_SPAWN_ONCE));
    this->invisibleRocketThrustEmitter->SetNumParticleLives(0); // Make initial lives zero so the emitter doesn't go off right as the rocket takes off
    this->invisibleRocketThrustEmitter->SetInitialSpd(ESPInterval(
        0.9f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_BEFORE_THRUST,
        0.9f*PaddleRemoteControlRocketProjectile::MAX_VELOCITY_WITH_THRUST));
    this->invisibleRocketThrustEmitter->SetParticleLife(ESPInterval(2.0f, 3.0f));
    this->invisibleRocketThrustEmitter->SetEmitAngleInDegrees(45);
    this->invisibleRocketThrustEmitter->SetRadiusDeviationFromCenter(
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f),
        ESPInterval(0.0f), 
        ESPInterval(0.0f, PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT/2.0f));
    this->invisibleRocketThrustEmitter->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
    this->invisibleRocketThrustEmitter->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
    this->invisibleRocketThrustEmitter->SetEmitPosition(Point3D(0, 0, 0));
    this->invisibleRocketThrustEmitter->AddEffector(&this->particleFader);
    this->invisibleRocketThrustEmitter->AddEffector(&this->particleLargeGrowth);
    this->invisibleRocketThrustEmitter->SetParticles(20, &this->invisibleThrustEffect);
}

RocketMesh::~RocketMesh() {
	this->rocketProjectiles.clear();
	
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleRocketMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleRemoteControlRocketMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->turretRocketMesh);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sparkTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->cloudNormalTex);
    assert(success);

    for (std::vector<Texture2D*>::iterator iter = this->smokeTextures.begin();
         iter != this->smokeTextures.end(); ++iter) {

        success = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        assert(success);	
    }
    this->smokeTextures.clear();

    UNUSED_VARIABLE(success);

	delete this->rocketGlowEmitter;
	this->rocketGlowEmitter = NULL;
    delete this->rocketThrustBurstEmitter;
    this->rocketThrustBurstEmitter = NULL;
    delete this->rocketThrustingSparksEmitter;
    this->rocketThrustingSparksEmitter = NULL;
    delete this->rocketThrustHyperBurnEmitter;
    this->rocketThrustHyperBurnEmitter = NULL;
    delete this->invisibleRocketThrustEmitter;
    this->invisibleRocketThrustEmitter = NULL;
}

// Activates the paddle rocket mesh so that it gets drawn based on the given projectile
void RocketMesh::ActivateRocket(const RocketProjectile* rocketProjectile) {
    std::pair<std::set<const RocketProjectile*>::iterator, bool> insertResult = this->rocketProjectiles.insert(rocketProjectile);
    assert(insertResult.second);

    if (rocketProjectile->GetType() == Projectile::PaddleRemoteCtrlRocketBulletProjectile) {
        this->ResetRemoteControlRocketEmitters();
    }
}

// Deactivates the rocket projectile - stops it from being drawn until next time it is activated
void RocketMesh::DeactivateRocket(const RocketProjectile* rocketProjectile) {
    size_t numErased = this->rocketProjectiles.erase(rocketProjectile);
    UNUSED_VARIABLE(numErased);
    assert(numErased == 1);

    if (rocketProjectile->GetType() == Projectile::PaddleRemoteCtrlRocketBulletProjectile) {
        this->ResetRemoteControlRocketEmitters();
    }
}

void RocketMesh::ResetRemoteControlRocketEmitters() {
    this->rocketThrustBurstEmitter->SetNumParticleLives(0);
    this->rocketThrustingSparksEmitter->SetNumParticleLives(0);
    this->rocketThrustHyperBurnEmitter->SetNumParticleLives(0);
    this->rocketThrustBurstEmitter->Reset();
    this->rocketThrustingSparksEmitter->Reset();
    this->rocketThrustHyperBurnEmitter->Reset();

    this->invisibleRocketThrustEmitter->SetNumParticleLives(0);
    this->invisibleRocketThrustEmitter->Reset();
}

// Draw the rocket - if it's currently activated
void RocketMesh::Draw(double dT, const PlayerPaddle& paddle, const Camera& camera, 
                      const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                      const BasicPointLight& ballLight, const Texture2D* sceneTex) {
    
    this->invisibleEffect.SetFBOTexture(sceneTex);

    bool rocketIsOnPaddle = paddle.HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle);
    if (rocketIsOnPaddle || !this->rocketProjectiles.empty()) {
	    this->rocketGlowEmitter->Tick(dT);
    }

    if (rocketIsOnPaddle) {

        glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        // Draw the rocket, mounted on the paddle
        const Point2D& paddleCenter = paddle.GetCenterPosition();
        float rocketHeight = paddle.GetPaddleScaleFactor() * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;
        float rocketZRotation = paddle.GetZRotation();
        Vector2D rocketDisplacement = (paddle.GetHalfHeight() + 0.5f * rocketHeight) * paddle.GetUpVector();

        glPushMatrix();
        glTranslatef(paddleCenter[0], paddleCenter[1], 0.0f);
        glRotatef(rocketZRotation, 0.0f, 0.0f, 1.0f);        
        glTranslatef(rocketDisplacement[0], rocketDisplacement[1], 0.0f);
        glScalef(paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor());

        // The rocket will be invisible under special circumstances...
        CgFxEffectBase* replacementMat = NULL;
        if (paddle.HasSpecialStatus(PlayerPaddle::InvisibleRocketStatus)) {
            replacementMat = &this->invisibleEffect;
            glDepthMask(GL_FALSE);
        }
        else {
            this->rocketGlowEmitter->SetParticleRotation(rocketZRotation);
            this->rocketGlowEmitter->Draw(camera);
        }

        // Specifics for drawing the particular type of rocket...
        if (paddle.HasPaddleType(PlayerPaddle::RocketPaddle)) {
            this->paddleRocketMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
        }
        else {
            assert(paddle.HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle));
            this->paddleRemoteControlRocketMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
        }
         
        glPopMatrix();

        glPopAttrib();
        debug_opengl_state();
	}

    static float scaleFactor   = 1.0f;
    static float currYRotation = 0.0f;
    static float currZRotation = 0.0f;

	for (std::set<const RocketProjectile*>::iterator iter = this->rocketProjectiles.begin();
         iter != this->rocketProjectiles.end(); ++iter) {

		const RocketProjectile* rocketProjectile = *iter;
		assert(rocketProjectile != NULL);

        glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);

        CgFxEffectBase* replacementMat = NULL;
        if (rocketProjectile->GetIsInvisible()) {
            replacementMat = &this->invisibleEffect;
            glDepthMask(GL_FALSE);
        }

		if (rocketProjectile->IsLoadedInCannonBlock()) {
            // Keep ticking and drawing certain rocket effects, even if it's inside a cannon
            this->TickAndDrawExtraneousRocketEffects(rocketProjectile, dT, camera);
        }
        else {
            // Draw the rocket if it's not inside a cannon block...
            switch (rocketProjectile->GetType()) {
                case Projectile::PaddleRocketBulletProjectile:
			        this->DrawBasicRocket(rocketProjectile, this->paddleRocketMesh, camera, keyLight, fillLight, ballLight, replacementMat);
                    break;
                case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
                    assert(dynamic_cast<const PaddleRemoteControlRocketProjectile*>(rocketProjectile) != NULL);
                    this->DrawRemoteControlRocket(static_cast<const PaddleRemoteControlRocketProjectile*>(rocketProjectile), 
                        dT, camera, keyLight, fillLight, ballLight, replacementMat);
                    break;
                case Projectile::RocketTurretBulletProjectile:
                case Projectile::BossRocketBulletProjectile:
                    this->DrawBasicRocket(rocketProjectile, this->turretRocketMesh, camera, keyLight, fillLight, ballLight, NULL);
                    break;
                default:
                    assert(false);
                    break;
            }
        }

        glPopAttrib();
	}

    debug_opengl_state();
}

void RocketMesh::ApplyRocketThrust(const PaddleRemoteControlRocketProjectile& rocket) {
    if (rocket.GetIsInvisible()) {
        this->invisibleRocketThrustEmitter->SetNumParticleLives(1);
        this->invisibleRocketThrustEmitter->Reset();
    }
    else {
        this->rocketThrustBurstEmitter->SetNumParticleLives(1);
        this->rocketThrustBurstEmitter->Reset();
        this->rocketThrustingSparksEmitter->Reset();
        this->rocketThrustHyperBurnEmitter->Reset();
    }
}

void RocketMesh::DrawBasicRocket(const RocketProjectile* rocket, Mesh* rocketMesh, 
                                 const Camera& camera, const BasicPointLight& keyLight, 
                                 const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                                 CgFxEffectBase* replacementMat) {
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Grab positioning / orienting values from the rocket projectile 
    // so we know where/how to draw the rocket
    const Vector2D& rocketDir = rocket->GetVelocityDirection();
    float currZRotation = Trig::radiansToDegrees(-M_PI_DIV2 + atan2(rocketDir[1], rocketDir[0]));
    this->rocketGlowEmitter->SetParticleRotation(ESPInterval(-currZRotation));

    const Point2D& rocketPos = rocket->GetPosition();

    glPushMatrix();
    glTranslatef(rocketPos[0], rocketPos[1], rocket->GetZOffset());

    float scaleFactor = rocket->GetVisualScaleFactor();

    if (!rocket->GetIsInvisible()) {
        this->rocketGlowEmitter->SetParticleSize(
           ESPInterval(2.0f*rocket->GetWidth()),
           ESPInterval(1.6f*rocket->GetHeight()));

        this->rocketGlowEmitter->Draw(camera);
    }

    // The rocket may not always be firing upwards, we need to rotate it to suit
    // its current direction, we also need to spin it on that axis
    glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(rocket->GetYRotation()), Vector3D(rocketDir, 0.0f)).begin());
    glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    rocketMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
    glPopMatrix();
}

void RocketMesh::DrawRemoteControlRocket(const PaddleRemoteControlRocketProjectile* rocket, double dT, const Camera& camera, 
                                         const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                                         const BasicPointLight& ballLight, CgFxEffectBase* replacementMat) {

    float flashingAmt = rocket->GetCurrentFlashingAmount();
    float oneMinusFlashingAmt = 1.0f - flashingAmt;
    glColor4f(std::min<float>(1.0f, 1.0f + flashingAmt), oneMinusFlashingAmt, oneMinusFlashingAmt, 1.0f);

    // Grab positioning / orienting values from the rocket projectile 
    // so we know where/how to draw the rocket
    const Vector2D& rocketDir = rocket->GetVelocityDirection();
    float currZRotation = Trig::radiansToDegrees(-M_PI_DIV2 + atan2(rocketDir[1], rocketDir[0]));
    this->rocketGlowEmitter->SetParticleRotation(ESPInterval(-currZRotation));

    const Point2D& rocketPos = rocket->GetPosition();
    float scaleFactor = rocket->GetVisualScaleFactor();

    glPushMatrix();
    glTranslatef(rocketPos[0], rocketPos[1], rocket->GetZOffset());

    Vector3D oppositeRocketDir(-rocketDir);
    Point3D emitPos = Point3D(0, 0, rocket->GetZOffset()) + (rocket->GetHalfHeight() * oppositeRocketDir);

    if (rocket->GetIsInvisible()) {
        if (rocket->GetCurrentAppliedThrustAmount() > 0.0f) {
            this->invisibleRocketThrustEmitter->SetNumParticleLives(1);
        }
        else {
            this->invisibleRocketThrustEmitter->SetNumParticleLives(0);
        }

        this->invisibleRocketThrustEmitter->SetParticleSize(ESPInterval(0.75f * rocket->GetWidth(), 2.0f * rocket->GetWidth()));
        this->invisibleRocketThrustEmitter->SetEmitDirection(oppositeRocketDir);
        this->invisibleRocketThrustEmitter->SetParticleSize(ESPInterval(rocket->GetWidth() / 1.5f, 1.5f * rocket->GetWidth()));
        this->invisibleRocketThrustEmitter->SetEmitPosition(emitPos);
        this->invisibleRocketThrustEmitter->Tick(dT);
        this->invisibleRocketThrustEmitter->Draw(camera);
    }
    else {
        // Special case for the remote control rocket, since the camera rotates with it we
        // don't need to rotate the glow effect on it
        this->rocketGlowEmitter->Draw(camera);

        if (rocket->GetCurrentAppliedThrustAmount() > 0.0f) {
            
            this->rocketThrustBurstEmitter->SetNumParticleLives(1);

            this->rocketThrustingSparksEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
            this->rocketThrustingSparksEmitter->SetEmitDirection(oppositeRocketDir);
            this->rocketThrustingSparksEmitter->SetInitialSpd(ESPInterval(0.33f*rocket->GetVelocityMagnitude(), 0.66f*rocket->GetVelocityMagnitude()));
            this->rocketThrustingSparksEmitter->SetParticleSize(ESPInterval(0.3f*rocket->GetWidth(), 0.75f*rocket->GetWidth()));
            this->rocketThrustingSparksEmitter->SetEmitPosition(emitPos);

            this->rocketThrustHyperBurnEmitter->SetNumParticleLives(ESPParticle::INFINITE_PARTICLE_LIVES);
            this->rocketThrustHyperBurnEmitter->SetEmitDirection(oppositeRocketDir);
            this->rocketThrustHyperBurnEmitter->SetInitialSpd(ESPInterval(0.3f*rocket->GetVelocityMagnitude(), 0.6f*rocket->GetVelocityMagnitude()));
            this->rocketThrustHyperBurnEmitter->SetParticleSize(ESPInterval(0.75f*rocket->GetWidth(), 0.95f*rocket->GetWidth()));
            this->rocketThrustHyperBurnEmitter->SetEmitPosition(emitPos);
        }
        else {
            this->rocketThrustBurstEmitter->SetNumParticleLives(0);
            this->rocketThrustingSparksEmitter->SetNumParticleLives(0);
            this->rocketThrustHyperBurnEmitter->SetNumParticleLives(0);
        }

        this->rocketThrustHyperBurnEmitter->Tick(dT);
        this->rocketThrustHyperBurnEmitter->Draw(camera);

        this->rocketThrustingSparksEmitter->Tick(dT);
        this->rocketThrustingSparksEmitter->Draw(camera);

        this->rocketThrustBurstEmitter->SetParticleSize(ESPInterval(0.75f * rocket->GetWidth(), 2.0f * rocket->GetWidth()));
        this->rocketThrustBurstEmitter->SetEmitDirection(oppositeRocketDir);
        this->rocketThrustBurstEmitter->SetParticleSize(ESPInterval(rocket->GetWidth() / 1.5f, 1.5f * rocket->GetWidth()));
        this->rocketThrustBurstEmitter->SetEmitPosition(emitPos);
        this->rocketThrustBurstEmitter->Tick(dT);
        this->rocketThrustBurstEmitter->Draw(camera);
    }

    // The rocket may not always be firing upwards, we need to rotate it to suit
    // its current direction, we also need to spin it on that axis
    glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(rocket->GetYRotation()), Vector3D(rocketDir, 0.0f)).begin());
    glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    this->paddleRemoteControlRocketMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);

    glPopMatrix();
}

void RocketMesh::TickAndDrawExtraneousRocketEffects(const RocketProjectile* rocket, double dT, const Camera& camera) {
    switch (rocket->GetType()) {

        case Projectile::PaddleRemoteCtrlRocketBulletProjectile: {
            const Point2D& rocketPos = rocket->GetPosition();

            glPushMatrix();
            glTranslatef(rocketPos[0], rocketPos[1], rocket->GetZOffset());

            if (rocket->GetIsInvisible()) {
                this->invisibleRocketThrustEmitter->Tick(dT);
                this->invisibleRocketThrustEmitter->Draw(camera);
            }
            else {
                this->rocketThrustBurstEmitter->Tick(dT);
                this->rocketThrustBurstEmitter->Draw(camera);
            }

            glPopMatrix();
            break;
        }

        default:
            break;
    }
}

// Private helper method to load the rocket mesh
void RocketMesh::LoadMeshes() {
	assert(this->paddleRocketMesh == NULL);
    assert(this->paddleRemoteControlRocketMesh == NULL);
    assert(this->turretRocketMesh == NULL);

    this->paddleRocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->PADDLE_ROCKET_MESH);
    this->paddleRemoteControlRocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->PADDLE_REMOTE_CONTROL_ROCKET_MESH);
    this->turretRocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(
        GameViewConstants::GetInstance()->TURRET_ROCKET_MESH);

    assert(this->paddleRocketMesh != NULL);
    assert(this->paddleRemoteControlRocketMesh != NULL);
    assert(this->turretRocketMesh != NULL);
}