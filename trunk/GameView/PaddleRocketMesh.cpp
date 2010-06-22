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
#include "../GameModel/PaddleRocketProjectile.h"
#include "../GameModel/PlayerPaddle.h"

PaddleRocketMesh::PaddleRocketMesh() : rocketMesh(NULL), rocketProjectile(NULL) {
	this->LoadMesh();
}

PaddleRocketMesh::~PaddleRocketMesh() {
	this->Deactivate();
	ResourceManager::GetInstance()->ReleaseMeshResource(this->rocketMesh);
}

// Draw the rocket - if it's currently activated
void PaddleRocketMesh::Draw(const PlayerPaddle& paddle, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) {
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	if ((paddle.GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		// Draw the rocket, mounted on the paddle
		const Point2D& paddleCenter = paddle.GetCenterPosition();
		
		glPushMatrix();
		glTranslatef(paddleCenter[0], paddleCenter[1] + paddle.GetHalfHeight() + PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT / 2.0f, 0.0f);
		this->rocketMesh->Draw(camera, keyLight, fillLight, ballLight);
		glPopMatrix();

		return;
	}
	
	// Nothing to draw if no rocket projectile is currently active
	if (this->rocketProjectile == NULL) {
		return;
	}
	
	// Grab positioning / orienting values from the rocket projectile 
	// so we know where/how to draw the rocket
	float currYRotation = this->rocketProjectile->GetYRotation();
	const Point2D& rocketPos = this->rocketProjectile->GetPosition();

	glPushMatrix();
	glTranslatef(rocketPos[0], rocketPos[1], 0.0f);
	glRotatef(currYRotation, 0.0f, 1.0f, 0.0f);
	this->rocketMesh->Draw(camera, keyLight, fillLight, ballLight);
	glPopMatrix();
}

// Private helper method to load the rocket mesh
void PaddleRocketMesh::LoadMesh() {
	assert(this->rocketMesh == NULL);
	this->rocketMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_ROCKET_MESH);
	assert(this->rocketMesh != NULL);
}