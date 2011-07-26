/**
 * Projectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Projectile.h"
#include "LevelPiece.h"
#include "GameEventManager.h"
#include "PaddleLaserProjectile.h"
#include "PaddleRocketProjectile.h"
#include "BallLaserProjectile.h"

// Projectile ====================================================================================================================

Projectile::Projectile(const Point2D& spawnLoc, float width, float height) : 
position(spawnLoc), lastThingCollidedWith(NULL), currWidth(width), currHeight(height) {
}

Projectile::Projectile(const Projectile& copy) : position(copy.position),
lastThingCollidedWith(copy.lastThingCollidedWith), currWidth(copy.currWidth), currHeight(copy.currHeight),
velocityDir(copy.velocityDir), velocityMag(copy.velocityMag), rightVec(copy.rightVec) {
}

Projectile::~Projectile() {
}

/**
 * Static factory creator pattern method for making projectiles based on the given type.
 */
Projectile* Projectile::CreateProjectileFromType(ProjectileType type, const Point2D& spawnLoc) {
	switch (type) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaserProjectile(spawnLoc);
        case Projectile::BallLaserBulletProjectile:
            return new BallLaserProjectile(spawnLoc);
		default:
			assert(false);
			break;
	}

	return NULL;
}

Projectile* Projectile::CreateProjectileFromCopy(const Projectile* p) {
    assert(p != NULL);
	switch (p->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			return new PaddleLaserProjectile(*static_cast<const PaddleLaserProjectile*>(p));
        case Projectile::BallLaserBulletProjectile:
            return new BallLaserProjectile(*static_cast<const BallLaserProjectile*>(p));
		default:
			assert(false);
			break;
	}

	return NULL;
}