/**
 * Projectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"

#include "BoundingLines.h"

class LevelPiece;

/**
 * An 'abstract class' for representing in-game projectiles.
 */
class Projectile {
public:
	enum ProjectileType { PaddleLaserBulletProjectile, CollateralBlockProjectile };
protected:
	Projectile(ProjectileType type, const Point2D& spawnLoc, float width, float height);
	Projectile(const Projectile& copy);
	
	ProjectileType type;

	float currWidth;
	float currHeight;
	Point2D position;			// Position of the projectile in game units
	Vector2D velocityDir;	// Velocity direction of the projectile
	Vector2D rightVec;		// Unit vector pointing outwards to the right of the particle, perpendicular to the velocity direction
	float velocityMag;		// Velocity magnitude of the projectile in game units / second
	LevelPiece* lastPieceCollidedWith;

public:
	virtual ~Projectile();
	virtual void Tick(double seconds) = 0;
	virtual BoundingLines BuildBoundingLines() const = 0;

	float GetWidth() const { return this->currWidth; }
	float GetHeight() const { return  this->currHeight; }
	float GetHalfWidth() const { return 0.5f * this->currWidth; }
	float GetHalfHeight() const { return  0.5f * this->currHeight; }
	void SetWidth(float width) { this->currWidth = width; }
	void SetHeight(float height) { this->currHeight = height; }

	ProjectileType GetType() const { return this->type; }
	const Point2D& GetPosition() const { return this->position; }

	const Vector2D& GetVelocityDirection() const { return this->velocityDir; }
	float GetVelocityMagnitude() const { return this->velocityMag; }

	virtual void SetPosition(const Point2D& pos) { this->position = pos; } 
	void SetVelocity(const Vector2D& velocityDir, float velocityMag) { 
		this->velocityDir = velocityDir;
		this->velocityMag = velocityMag;

		// Set the right vector... (short cut which is a 90 degree rotation about the z axis)
		this->rightVec = Vector2D(this->velocityDir[1], -this->velocityDir[0]);
	}

	Vector2D GetRightVectorDirection() const { return this->rightVec; }

	// Functionality for storing the last level piece that the particle collided with
	// and for querying it
	void SetLastLevelPieceCollidedWith(LevelPiece* p) { this->lastPieceCollidedWith = p; }
	bool IsLastLevelPieceCollidedWith(LevelPiece* p) { return this->lastPieceCollidedWith == p; }

	static Projectile* CreateProjectile(ProjectileType type, const Point2D& spawnLoc);
};

#endif