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
#include "PaddleRocketProjectile.h"

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

/**
 * Build an 2D Axis-Aligned Bounding Box for this projectile.
 */
/*
Collision::AABB2D Projectile::BuildAABB2D() const {
	static const Vector2D unitX(1, 0);
	static const Vector2D unitY(0, 1);
	
	// Create the set of points representing the 4 outer points of this projectile
	const Vector2D UP_DIR			    = this->GetHalfHeight() * this->GetVelocityDirection();
	const Vector2D FULL_RIGHT_DIR = this->GetHalfWidth() * this->GetRightVectorDirection();

	Point2D projectilePts[4];
	projectilePts[0] = this->GetPosition() + UP_DIR + RIGHT_DIR;
	projectilePts[1] = this->GetPosition() + UP_DIR - RIGHT_DIR;
	projectilePts[2] = this->GetPosition() - UP_DIR + RIGHT_DIR;
	projectilePts[3] = this->GetPosition() - UP_DIR - RIGHT_DIR;

	// Now figure out what the min and max boundries of the AABB will be by projecting
	// those points onto the x and y axes...
	

	return Collision::AABB2D(
}
*/