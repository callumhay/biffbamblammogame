/**
 * PaddleMineProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleMineProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

const float PaddleMineProjectile::PADDLEMINE_HEIGHT_DEFAULT = 0.8f;
const float PaddleMineProjectile::PADDLEMINE_WIDTH_DEFAULT  = 0.8f;

const Vector2D PaddleMineProjectile::MINE_DEFAULT_VELOCITYDIR = Vector2D(0, 1);
const Vector2D PaddleMineProjectile::MINE_DEFAULT_RIGHTDIR    = Vector2D(1, 0);

PaddleMineProjectile::PaddleMineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL) {

    if (velDir.IsZero()) {
        assert(false);
        this->velocityDir = MINE_DEFAULT_VELOCITYDIR;
        this->rightVec    = MINE_DEFAULT_RIGHTDIR;
    }
    else {
        this->velocityDir = velDir;
        this->rightVec    = Vector2D::Rotate(-90, velDir);
        this->velocityDir.Normalize();
        this->rightVec.Normalize();
    }

	this->velocityMag      = 0.0f;
	this->currRotation    = 0.0f;
	this->currRotationSpd = 0.0f;
}

PaddleMineProjectile::PaddleMineProjectile(const PaddleMineProjectile& copy) : Projectile(copy),
cannonBlock(copy.cannonBlock), currRotationSpd(copy.currRotationSpd),
currRotation(copy.currRotation) {

}

PaddleMineProjectile::~PaddleMineProjectile() {
}

void PaddleMineProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock != NULL) {
		// 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
		// it has fired the rocket
		bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);
		if (cannonHasFired) {
			// Set the velocity in the direction the cannon has fired in
            this->velocityMag = 0.25f * this->GetMaxVelocityMagnitude();
			this->velocityDir = this->cannonBlock->GetCurrentCannonDirection();
			this->position = this->position + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

			// EVENT: Mine has officially been fired from the cannon
			GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);

			this->cannonBlock = NULL;
		}
	}
	else {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 60.0f);

		// Update the rocket's velocity and position
        float dA = seconds * this->GetAccelerationMagnitude();
        this->velocityMag = std::min<float>(this->GetMaxVelocityMagnitude(), this->velocityMag + dA);
		float dV = seconds * this->velocityMag;
		this->SetPosition(this->GetPosition() + dV * this->velocityDir);

		// Update the rocket's rotation
        dA = seconds * this->GetRotationAccelerationMagnitude();
        this->currRotationSpd = std::min<float>(this->GetMaxRotationVelocityMagnitude(), this->currRotationSpd + dA);
		dV = static_cast<float>(seconds * this->currRotationSpd);
		this->currRotation += dV;
	}
}

BoundingLines PaddleMineProjectile::BuildBoundingLines() const {
	// If the mine is inside a cannon block it has no bounding lines...
	if (this->IsLoadedInCannonBlock()) {
		return BoundingLines();
	}

	const Vector2D& UP_DIR    = this->GetVelocityDirection();
	const Vector2D& RIGHT_DIR = this->GetRightVectorDirection();

	Point2D topRight = this->GetPosition() + this->GetHalfHeight()*UP_DIR + 0.9f*this->GetHalfWidth()*RIGHT_DIR;
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

void PaddleMineProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag = 0.0f;
	this->position    = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;

	// EVENT: The mine is officially loaded into the cannon block
	GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}