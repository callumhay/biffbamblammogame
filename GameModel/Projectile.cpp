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
		case Projectile::PaddleLaserProjectile:
			return new PaddleLaser(spawnLoc);
		default:
			assert(false);
			break;
	}

	return NULL;
}

// PaddleLaser ===================================================================================================================
const Vector2D PaddleLaser::PADDLELASER_VELOCITYDIR	= Vector2D(0, 1);	// Velocity of laser projectile in game units / second
const float PaddleLaser::PADDLELASER_VELOCITYMAG	= 10.0f;
const float PaddleLaser::PADDLELASER_HEIGHT				= 1.2f;							// Height of a laser projectile in game units
const float PaddleLaser::PADDLELASER_WIDTH				= 0.5f;							// Width of a laser projectile in game units 
const float PaddleLaser::PADDLELASER_HALF_HEIGHT	= PADDLELASER_HEIGHT / 2.0f;
const float PaddleLaser::PADDLELASER_HALF_WIDTH		= PADDLELASER_WIDTH  / 2.0f;

PaddleLaser::PaddleLaser(const Point2D& spawnLoc) : Projectile(Projectile::PaddleLaserProjectile, spawnLoc) {
	this->velocityDir = PADDLELASER_VELOCITYDIR;
	this->velocityMag = PADDLELASER_VELOCITYMAG;
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