/**
 * RocketMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
paddleRocketMesh(NULL), paddleRemoteControlRocketMesh(NULL), turretRocketMesh(NULL),
rocketGlowEmitter(NULL), pulseEffector(0,0) {

	this->LoadMeshes();

	this->glowTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Bilinear));
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
	this->rocketGlowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->rocketGlowEmitter->SetEmitPosition(Point3D(0, 0, 0));
	this->rocketGlowEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.70f));
	this->rocketGlowEmitter->AddEffector(&this->pulseEffector);
	this->rocketGlowEmitter->SetParticles(1, this->glowTex);
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
    UNUSED_VARIABLE(success);

	delete this->rocketGlowEmitter;
	this->rocketGlowEmitter = NULL;
}

// Draw the rocket - if it's currently activated
void RocketMesh::Draw(double dT, const PlayerPaddle& paddle, const Camera& camera, 
                      const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                      const BasicPointLight& ballLight) {

    bool rocketIsOnPaddle = paddle.HasPaddleType(PlayerPaddle::RocketPaddle | PlayerPaddle::RemoteControlRocketPaddle);
    if (rocketIsOnPaddle || !this->rocketProjectiles.empty()) {
	    this->rocketGlowEmitter->Tick(dT);
    }

    if (rocketIsOnPaddle) {
        glPushAttrib(GL_CURRENT_BIT);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        // Draw the rocket, mounted on the paddle
        const Point2D& paddleCenter = paddle.GetCenterPosition();

        glPushMatrix();
        float rocketHeight = paddle.GetPaddleScaleFactor() * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;

        glTranslatef(paddleCenter[0], paddleCenter[1] + paddle.GetHalfHeight() + 0.5f * rocketHeight, 0.0f);
        glScalef(paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor());

        this->rocketGlowEmitter->SetParticleRotation(ESPInterval(0.0f));
        this->rocketGlowEmitter->Draw(camera);

        // Specifics for drawing the particular type of rocket...
        if (paddle.HasPaddleType(PlayerPaddle::RocketPaddle)) {
            this->paddleRocketMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        else {
            assert(paddle.HasPaddleType(PlayerPaddle::RemoteControlRocketPaddle));
            this->paddleRemoteControlRocketMesh->Draw(camera, keyLight, fillLight, ballLight);
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

		// Draw the rocket if it's not inside a cannon block...
		if (!rocketProjectile->IsLoadedInCannonBlock()) {

			glPushAttrib(GL_CURRENT_BIT);

            switch (rocketProjectile->GetType()) {
                case Projectile::PaddleRocketBulletProjectile:
			        this->DrawBasicRocket(rocketProjectile, this->paddleRocketMesh, camera, keyLight, fillLight, ballLight);
                    break;
                case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
                    assert(dynamic_cast<const PaddleRemoteControlRocketProjectile*>(rocketProjectile) != NULL);
                    this->DrawRemoteControlRocket(static_cast<const PaddleRemoteControlRocketProjectile*>(rocketProjectile), 
                        camera, keyLight, fillLight, ballLight);
                    break;
                case Projectile::RocketTurretBulletProjectile:
                case Projectile::BossRocketBulletProjectile:
                    this->DrawBasicRocket(rocketProjectile, this->turretRocketMesh, camera, keyLight, fillLight, ballLight);
                    break;
                default:
                    assert(false);
                    break;
            }

			glPopAttrib();
        }
	}

    debug_opengl_state();
}

void RocketMesh::DrawBasicRocket(const RocketProjectile* rocket, Mesh* rocketMesh, 
                                 const Camera& camera, const BasicPointLight& keyLight, 
                                 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    
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
    this->rocketGlowEmitter->SetParticleSize(
       ESPInterval(2.0f*rocket->GetWidth()),
       ESPInterval(1.6f*rocket->GetHeight()));

    this->rocketGlowEmitter->Draw(camera);

    // The rocket may not always be firing upwards, we need to rotate it to suit
    // its current direction, we also need to spin it on that axis
    glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(rocket->GetYRotation()), Vector3D(rocketDir, 0.0f)).begin());
    glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    rocketMesh->Draw(camera, keyLight, fillLight, ballLight);
    glPopMatrix();
}

void RocketMesh::DrawRemoteControlRocket(const PaddleRemoteControlRocketProjectile* rocket, const Camera& camera, 
                                         const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                                         const BasicPointLight& ballLight) {

    float flashingAmt = rocket->GetFlashingAmount();
    float oneMinusFlashingAmt = 1.0f - flashingAmt;
    glColor4f(std::min<float>(1.0f, 1.0f + flashingAmt), oneMinusFlashingAmt, oneMinusFlashingAmt, 1.0f);

    // Grab positioning / orienting values from the rocket projectile 
    // so we know where/how to draw the rocket
    const Vector2D& rocketDir = rocket->GetVelocityDirection();
    float currZRotation = Trig::radiansToDegrees(-M_PI_DIV2 + atan2(rocketDir[1], rocketDir[0]));

    const Point2D& rocketPos = rocket->GetPosition();

    glPushMatrix();
    glTranslatef(rocketPos[0], rocketPos[1], rocket->GetZOffset());

    // Special case for the remote control rocket, since the camera rotates with it we
    // don't need to rotate the glow effect on it
    this->rocketGlowEmitter->SetParticleRotation(ESPInterval(0.0f));

    float scaleFactor = rocket->GetVisualScaleFactor();
    this->rocketGlowEmitter->SetParticleSize(ESPInterval(2.0f*rocket->GetWidth()), ESPInterval(1.6f*rocket->GetHeight()));
    this->rocketGlowEmitter->Draw(camera);

    // The rocket may not always be firing upwards, we need to rotate it to suit
    // its current direction, we also need to spin it on that axis
    glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(rocket->GetYRotation()), Vector3D(rocketDir, 0.0f)).begin());
    glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
    glScalef(scaleFactor, scaleFactor, scaleFactor);

    this->paddleRemoteControlRocketMesh->Draw(camera, keyLight, fillLight, ballLight);

    glPopMatrix();
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