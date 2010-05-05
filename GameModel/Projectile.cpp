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
#include "PaddleLaser.h"

// Projectile ====================================================================================================================

Projectile::Projectile(ProjectileType type, const Point2D& spawnLoc, float width, float height) : 
type(type), position(spawnLoc), lastPieceCollidedWith(NULL), currWidth(width), currHeight(height) {
}

Projectile::Projectile(const Projectile& copy) : type(copy.type), position(copy.position),
lastPieceCollidedWith(copy.lastPieceCollidedWith), currWidth(copy.currWidth), currHeight(copy.currHeight),
velocityDir(copy.velocityDir), velocityMag(copy.velocityMag), rightVec(copy.rightVec) {
}

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