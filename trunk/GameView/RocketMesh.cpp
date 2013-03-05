/**
 * RocketMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RocketMesh.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPPointEmitter.h"

#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/PlayerPaddle.h"

RocketMesh::RocketMesh() : 
paddleRocketMesh(NULL), turretRocketMesh(NULL), rocketGlowEmitter(NULL), pulseEffector(0,0) {
	this->LoadMesh();

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

	this->rocketGlowEmitter->Tick(dT);
	if ((paddle.GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			// Draw the rocket, mounted on the paddle
			const Point2D& paddleCenter = paddle.GetCenterPosition();
			
			glPushMatrix();
			float rocketHeight = paddle.GetPaddleScaleFactor() * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;

			glTranslatef(paddleCenter[0], paddleCenter[1] + paddle.GetHalfHeight() + 0.5f * rocketHeight, 0.0f);
			glScalef(paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor());
			this->rocketGlowEmitter->SetParticleRotation(ESPInterval(0.0));
			this->rocketGlowEmitter->Draw(camera);
			this->paddleRocketMesh->Draw(camera, keyLight, fillLight, ballLight);
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
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			// Grab positioning / orienting values from the rocket projectile 
			// so we know where/how to draw the rocket
			currYRotation = rocketProjectile->GetYRotation();
			const Vector2D& rocketDir = rocketProjectile->GetVelocityDirection();
			currZRotation = Trig::radiansToDegrees(-M_PI_DIV2 + atan2(rocketDir[1], rocketDir[0]));
			const Point2D& rocketPos = rocketProjectile->GetPosition();

			glPushMatrix();
			glTranslatef(rocketPos[0], rocketPos[1], rocketProjectile->GetZOffset());

			this->rocketGlowEmitter->SetParticleRotation(ESPInterval(-currZRotation));

            float scaleFactor = rocketProjectile->GetVisualScaleFactor();
            this->rocketGlowEmitter->SetParticleSize(
                ESPInterval(2.0f*rocketProjectile->GetWidth()),
                ESPInterval(1.6f*rocketProjectile->GetHeight()));

			this->rocketGlowEmitter->Draw(camera);

			// The rocket may not always be firing upwards, we need to rotate it to suit
			// its current direction, we also need to spin it on that axis
			glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(currYRotation), Vector3D(rocketDir, 0.0f)).begin());
			glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
			glScalef(scaleFactor, scaleFactor, scaleFactor);
            
            switch (rocketProjectile->GetType()) {
                case Projectile::PaddleRocketBulletProjectile:
			        this->paddleRocketMesh->Draw(camera, keyLight, fillLight, ballLight);
                    break;
                case Projectile::RocketTurretBulletProjectile:
                case Projectile::BossRocketBulletProjectile:
                    this->turretRocketMesh->Draw(camera, keyLight, fillLight, ballLight);
                    break;
                default:
                    assert(false);
                    break;
            }

			glPopMatrix();

			glPopAttrib();
			debug_opengl_state();
		}
	}
}

// Private helper method to load the rocket mesh
void RocketMesh::LoadMesh() {
	assert(this->paddleRocketMesh == NULL);
    assert(this->turretRocketMesh == NULL);

    this->paddleRocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_ROCKET_MESH);
	assert(this->paddleRocketMesh != NULL);

    this->turretRocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TURRET_ROCKET_MESH);
    assert(this->turretRocketMesh != NULL);
}