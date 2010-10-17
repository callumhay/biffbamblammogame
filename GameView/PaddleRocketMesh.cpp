/**
 * PaddleRocketMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleRocketMesh.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPPointEmitter.h"

#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/PlayerPaddle.h"

const float PaddleRocketMesh::GLOW_X_SIZE	= 1.2f * PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT;
const float PaddleRocketMesh::GLOW_Y_SIZE	= 1.1f * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;

PaddleRocketMesh::PaddleRocketMesh() : 
rocketMesh(NULL), rocketGlowEmitter(NULL), pulseEffector(0,0) {
	this->LoadMesh();

	this->glowTex = dynamic_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_CIRCLE_GRADIENT, Texture::Bilinear));
	assert(this->glowTex != NULL);
	
	ScaleEffect pulseSettings;
	pulseSettings.pulseGrowthScale = 1.8f;
	pulseSettings.pulseRate = 0.75f;
	this->pulseEffector = ESPParticleScaleEffector(pulseSettings);

	this->rocketGlowEmitter = new ESPPointEmitter();
	this->rocketGlowEmitter->SetSpawnDelta(ESPInterval(-1));
	this->rocketGlowEmitter->SetInitialSpd(ESPInterval(0));
	this->rocketGlowEmitter->SetParticleLife(ESPInterval(-1));
	this->rocketGlowEmitter->SetParticleSize(ESPInterval(GLOW_X_SIZE), 
		                                       ESPInterval(GLOW_Y_SIZE));
	this->rocketGlowEmitter->SetEmitAngleInDegrees(0);
	this->rocketGlowEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	this->rocketGlowEmitter->SetParticleAlignment(ESP::ScreenAligned);
	this->rocketGlowEmitter->SetEmitPosition(Point3D(0, 0, 0));
	this->rocketGlowEmitter->SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(0.70f));
	this->rocketGlowEmitter->AddEffector(&this->pulseEffector);
	this->rocketGlowEmitter->SetParticles(1, this->glowTex);
}

PaddleRocketMesh::~PaddleRocketMesh() {
	this->rocketProjectiles.clear();
	ResourceManager::GetInstance()->ReleaseMeshResource(this->rocketMesh);
	ResourceManager::GetInstance()->ReleaseTextureResource(this->glowTex);

	delete this->rocketGlowEmitter;
	this->rocketGlowEmitter = NULL;
}

// Draw the rocket - if it's currently activated
void PaddleRocketMesh::Draw(double dT, const PlayerPaddle& paddle, const Camera& camera, 
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
			this->rocketGlowEmitter->Draw(camera);
			this->rocketMesh->Draw(camera, keyLight, fillLight, ballLight);
			glPopMatrix();

			glPopAttrib();
			debug_opengl_state();
	}

	for (std::set<const PaddleRocketProjectile*>::iterator iter = this->rocketProjectiles.begin(); iter != this->rocketProjectiles.end(); ++iter) {
		const PaddleRocketProjectile* rocketProjectile = *iter;
		assert(rocketProjectile != NULL);

		// Draw the rocket if it's not inside a cannon block...
		if (!rocketProjectile->IsLoadedInCannonBlock()) {

			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			// Grab positioning / orienting values from the rocket projectile 
			// so we know where/how to draw the rocket
			float currYRotation = rocketProjectile->GetYRotation();
			const Vector2D& rocketDir = rocketProjectile->GetVelocityDirection();
			float currZRotation = Trig::radiansToDegrees(-M_PI_DIV2 + atan2(rocketDir[1], rocketDir[0]));
			const Point2D& rocketPos = rocketProjectile->GetPosition();

			glPushMatrix();
			glTranslatef(rocketPos[0], rocketPos[1], 0.0f);

			glPushMatrix();
			glScalef(paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor());
			this->rocketGlowEmitter->SetParticleRotation(ESPInterval(-currZRotation));
			this->rocketGlowEmitter->Draw(camera);
			glPopMatrix();

			// The rocket may not always be firing upwards, we need to rotate it to suit
			// its current direction, we also need to spin it on that axis
			glMultMatrixf(Matrix4x4::rotationMatrix(Trig::degreesToRadians(currYRotation), Vector3D(rocketDir, 0.0f)).begin());
			glRotatef(currZRotation, 0.0f, 0.0f, 1.0f);
			
			glScalef(paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor(), paddle.GetPaddleScaleFactor());
			this->rocketMesh->Draw(camera, keyLight, fillLight, ballLight);
			glPopMatrix();

			glPopAttrib();
			debug_opengl_state();
		}
	}
}

// Private helper method to load the rocket mesh
void PaddleRocketMesh::LoadMesh() {
	assert(this->rocketMesh == NULL);
	this->rocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_ROCKET_MESH);
	assert(this->rocketMesh != NULL);
}