/**
 * RocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RocketProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

const Vector2D RocketProjectile::ROCKET_DEFAULT_VELOCITYDIR = Vector2D(0, 1);
const Vector2D RocketProjectile::ROCKET_DEFAULT_RIGHTDIR    = Vector2D(1, 0);

const float RocketProjectile::DEFAULT_VISUAL_WIDTH  = 0.8f;
const float RocketProjectile::DEFAULT_VISUAL_HEIGHT = 1.5f;

RocketProjectile::RocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir,
                                   float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL) {

    if (rocketVelDir.IsZero()) {
        assert(false);
        this->velocityDir = ROCKET_DEFAULT_VELOCITYDIR;
        this->rightVec    = ROCKET_DEFAULT_RIGHTDIR;
    }
    else {
        this->velocityDir = rocketVelDir;
        this->rightVec    = Vector2D::Rotate(-90, rocketVelDir);
        this->velocityDir.Normalize();
        this->rightVec.Normalize();
    }

	this->velocityMag      = 0.0f;
	this->currYRotation    = 0.0f;
	this->currYRotationSpd = 0.0f;
}

RocketProjectile::RocketProjectile(const RocketProjectile& copy) : Projectile(copy),
cannonBlock(copy.cannonBlock), currYRotationSpd(copy.currYRotationSpd),
currYRotation(copy.currYRotation) {
}

RocketProjectile::~RocketProjectile() {
}

BoundingLines RocketProjectile::BuildBoundingLines() const {
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

/**
 * Executed every game tick, updates the velocities, 
 * position and rotation of the rocket projectile.
 */
void RocketProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock != NULL) {
		// 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
		// it has fired the rocket
		bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);
		if (cannonHasFired) {
			// Set the velocity in the direction the cannon has fired in
            this->velocityMag = 0.25f * this->GetMaxVelocityMagnitude();
			this->velocityDir = this->cannonBlock->GetCurrentCannonDirection();
			this->position = this->position + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

			// EVENT: Rocket has officially been fired from the cannon
			GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);

			this->cannonBlock = NULL;
		}
	}
	else {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 55.0f);

		// Update the rocket's velocity and position
        float dA = seconds * this->GetAccelerationMagnitude();
        this->velocityMag = std::min<float>(this->GetMaxVelocityMagnitude(), this->velocityMag + dA);
		float dV = seconds * this->velocityMag;
		this->SetPosition(this->GetPosition() + dV * this->velocityDir);

		// Update the rocket's rotation
        dA = seconds * this->GetRotationAccelerationMagnitude();
        this->currYRotationSpd = std::min<float>(this->GetMaxRotationVelocityMagnitude(), this->currYRotationSpd + dA);
		dV = static_cast<float>(seconds * this->currYRotationSpd);
		this->currYRotation += dV;
	}
}

void RocketProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag = 0.0f;
	this->position    = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;

	// EVENT: The rocket is officially loaded into the cannon block
	GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}