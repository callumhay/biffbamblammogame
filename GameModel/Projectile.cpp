/**
 * Projectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Projectile.h"
#include "LevelPiece.h"
#include "GameEventManager.h"

// Projectile ====================================================================================================================

Projectile::~Projectile() {
}

/**
 * Static factory creator pattern method for making projectiles based on the given type.
 */
Projectile* Projectile::CreateProjectile(ProjectileType type, const Point2D& spawnLoc) {
	switch (type) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaser(spawnLoc);
		default:
			assert(false);
			break;
	}

	return NULL;
}

// PaddleLaser ===================================================================================================================
const Vector2D PaddleLaser::PADDLELASER_VELOCITYDIR	= Vector2D(0, 1);	// Velocity of laser projectile in game units / second
const Vector2D PaddleLaser::PADDLELASER_RIGHTDIR	  = Vector2D(1, 0);
const float PaddleLaser::PADDLELASER_VELOCITYMAG	= 10.0f;
const float PaddleLaser::PADDLELASER_HEIGHT				= 1.2f;							// Height of a laser projectile in game units
const float PaddleLaser::PADDLELASER_WIDTH				= 0.5f;							// Width of a laser projectile in game units 
const float PaddleLaser::PADDLELASER_HALF_HEIGHT	= PADDLELASER_HEIGHT / 2.0f;
const float PaddleLaser::PADDLELASER_HALF_WIDTH		= PADDLELASER_WIDTH  / 2.0f;

PaddleLaser::PaddleLaser(const Point2D& spawnLoc) : 
Projectile(Projectile::PaddleLaserBulletProjectile, spawnLoc) {
	this->velocityDir = PADDLELASER_VELOCITYDIR;
	this->velocityMag = PADDLELASER_VELOCITYMAG;
	this->rightVec    = PADDLELASER_RIGHTDIR;
}

PaddleLaser::~PaddleLaser() {
}

/**
 * Tick function - executed every frame with given delta seconds. This will
 * update the position and other relevant information for this paddle laser projectile.
 */
void PaddleLaser::Tick(double seconds) {
	// Update the laser's position and bounding box
	Vector2D dV = seconds * this->velocityMag * this->velocityDir;
	this->position = this->position + dV;
}

/**
 * Obtain the bounding lines for this projectile for use in various collision detection tests.
 * Returns: Bounding lines for this projectile.
 */
BoundingLines PaddleLaser::BuildBoundingLines() const {
			const Vector2D UP_DIR			= this->GetVelocityDirection();
			const Vector2D RIGHT_DIR	= this->GetRightVectorDirection();

			Point2D topRight = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
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