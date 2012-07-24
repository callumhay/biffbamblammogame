/**
 * Projectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2012
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
class GameModel;
class SafetyNet;

/**
 * An 'abstract class' for representing in-game projectiles.
 */
class Projectile {
public:
	enum ProjectileType { PaddleLaserBulletProjectile, BallLaserBulletProjectile, LaserTurretBulletProjectile,
                          CollateralBlockProjectile, PaddleRocketBulletProjectile, RocketTurretBulletProjectile,
                          FireGlobProjectile, PaddleMineBulletProjectile };

	virtual ~Projectile();
	virtual void Tick(double seconds, const GameModel& model) = 0;
	virtual BoundingLines BuildBoundingLines() const          = 0;
	virtual ProjectileType GetType() const                    = 0;
    virtual float GetDamage() const                           = 0;
    virtual bool IsRocket() const                             = 0;
    virtual bool IsRefractableOrReflectable() const           = 0;

    virtual float GetZOffset() const { return 0.0f; }

    virtual bool BlastsThroughSafetyNets() const { return true; }
    virtual bool IsDestroyedBySafetyNets() const { return false; }

    virtual void SafetyNetCollisionOccurred(SafetyNet* safetyNet) { this->SetLastThingCollidedWith(safetyNet); };
    virtual void LevelPieceCollisionOccurred(LevelPiece* block)   { UNUSED_PARAMETER(block); };

    virtual bool ModifyLevelUpdate(double dT, GameModel&) { UNUSED_PARAMETER(dT); return false; };

    void AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec);

	float GetWidth() const { return this->currWidth; }
	float GetHeight() const { return  this->currHeight; }
	float GetHalfWidth() const { return 0.5f * this->currWidth; }
	float GetHalfHeight() const { return  0.5f * this->currHeight; }
	void SetWidth(float width) { this->currWidth = width; }
	void SetHeight(float height) { this->currHeight = height; }

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
	void SetVelocity(const Vector2D& velocity) { 
		this->velocityMag = Vector2D::Magnitude(velocity);
        assert(this->velocityMag > EPSILON);
        this->velocityDir = velocity / this->velocityMag;
        
		// Set the right vector... (short cut which is a 90 degree rotation about the z axis)
		this->rightVec = Vector2D(this->velocityDir[1], -this->velocityDir[0]);
	}
	Vector2D GetRightVectorDirection() const { return this->rightVec; }

	// Functionality for storing the last level piece that the particle collided with
	// and for querying it
	void SetLastThingCollidedWith(const void* p) { this->lastThingCollidedWith = p; }
	bool IsLastThingCollidedWith(const void* p) const { return this->lastThingCollidedWith == p; }

    static Projectile* CreateProjectileFromCopy(const Projectile* p);

	virtual bool GetIsActive() const {
		return true;
	}

protected:
	Projectile(const Point2D& spawnLoc, float width, float height);
	Projectile(const Projectile& copy);
	
	float currWidth;
	float currHeight;
	Point2D position;	    // Position of the projectile in game units
	Vector2D velocityDir;	// Velocity direction of the projectile
	Vector2D rightVec;		// Unit vector pointing outwards to the right of the particle, perpendicular to the velocity direction
	float velocityMag;		// Velocity magnitude of the projectile in game units / second
	const void* lastThingCollidedWith;

};


#endif