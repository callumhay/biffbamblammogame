/**
 * PaddleRocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleRocketProjectile.h"
#include "PlayerPaddle.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

const float PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT = 1.50f;
const float PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT  = 0.8f;

const Vector2D PaddleRocketProjectile::PADDLEROCKET_VELOCITYDIR	= Vector2D(0, 1);
const Vector2D PaddleRocketProjectile::PADDLEROCKET_RIGHTDIR    = Vector2D(1, 0);

const float PaddleRocketProjectile::ACCELERATION_MAG            = 2.0f;
const float PaddleRocketProjectile::ROTATION_ACCELERATION_MAG   = 60.0f;

const float PaddleRocketProjectile::MAX_VELOCITY_MAG            = 20.0f;
const float PaddleRocketProjectile::MAX_ROTATION_VELOCITY_MAG	= 400.0f;

PaddleRocketProjectile::PaddleRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL) {

    if (rocketVelDir.IsZero()) {
        assert(false);
        this->velocityDir = PADDLEROCKET_VELOCITYDIR; 
    }
    else {
        this->velocityDir   = rocketVelDir;
    }

	this->rightVec      = PADDLEROCKET_RIGHTDIR;
	this->velocityMag   = 0.0f;
	this->currYRotation = 0.0f;
	this->currYRotationSpd = 0.0f;
}

PaddleRocketProjectile::~PaddleRocketProjectile() {
}

/**
 * Executed every game tick, updates the velocities, 
 * position and rotation of the rocket projectile.
 */
void PaddleRocketProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock != NULL) {
		// 'Tick' the cannon to spin the ball around inside it... eventually the function will say
		// it has 'fired' the ball
		bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);
		if (cannonHasFired) {
			// Set the velocity in the direction the cannon has fired in
			this->velocityMag = 0.25f * PaddleRocketProjectile::MAX_VELOCITY_MAG;
			this->velocityDir = this->cannonBlock->GetCurrentCannonDirection();
			this->position = this->position + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

			// EVENT: Rocket has officially been fired from the cannon
			GameEventManager::Instance()->ActionRocketFiredFromCannon(*this, *this->cannonBlock);

			this->cannonBlock = NULL;
		}
	}
	else {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 55.0f);

		// Update the rocket's velocity and position
		float dA = seconds * PaddleRocketProjectile::ACCELERATION_MAG;
		this->velocityMag = std::min<float>(PaddleRocketProjectile::MAX_VELOCITY_MAG ,this->velocityMag + dA);
		float dV = seconds * this->velocityMag;
		this->SetPosition(this->GetPosition() + dV * this->velocityDir);

		// Update the rocket's rotation
		dA = seconds * PaddleRocketProjectile::ROTATION_ACCELERATION_MAG;
		this->currYRotationSpd = std::min<float>(PaddleRocketProjectile::MAX_ROTATION_VELOCITY_MAG, this->currYRotationSpd + dA);
		dV = static_cast<float>(seconds * this->currYRotationSpd);
		this->currYRotation += dV;
	}
}

BoundingLines PaddleRocketProjectile::BuildBoundingLines() const {
	// If the rocket is inside a cannon block it has no bounding lines...
	if (this->cannonBlock != NULL) {
		return BoundingLines();
	}

	const Vector2D& UP_DIR      = this->GetVelocityDirection();
	const Vector2D& RIGHT_DIR	= this->GetRightVectorDirection();

	Point2D topRight = this->GetPosition() + this->GetHalfHeight()*UP_DIR + 0.75f*this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
	Point2D topLeft = topRight - this->GetWidth()*RIGHT_DIR;
	Point2D bottomLeft = topLeft - this->GetHeight()*UP_DIR;

	std::vector<Collision::LineSeg2D> sideBounds;
	sideBounds.reserve(2);
	sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
	sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
	std::vector<Vector2D> normBounds;
	normBounds.resize(2);

	return BoundingLines(sideBounds, normBounds);
}

void PaddleRocketProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag = 0.0f;
	this->position = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;

	// EVENT: The rocket is officially loaded into the cannon block
	GameEventManager::Instance()->ActionRocketEnteredCannon(*this, *cannonBlock);
}