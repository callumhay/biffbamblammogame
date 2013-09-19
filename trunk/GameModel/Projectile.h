/**
 * Projectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "../BlammoEngine/IPositionObject.h"
#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"

#include "BoundingLines.h"


class LevelPiece;
class GameModel;
class SafetyNet;
class PlayerPaddle;
class Boss;
class BossBodyPart;

/**
 * An 'abstract class' for representing in-game projectiles.
 */
class Projectile : public IPositionObject {
public:
	enum ProjectileType { PaddleLaserBulletProjectile, BallLaserBulletProjectile, LaserTurretBulletProjectile,
                          CollateralBlockProjectile, PaddleRocketBulletProjectile, PaddleRemoteCtrlRocketBulletProjectile,
                          RocketTurretBulletProjectile, FireGlobProjectile, PaddleMineBulletProjectile, MineTurretBulletProjectile,
                          BossLaserBulletProjectile, BossRocketBulletProjectile, BossOrbBulletProjectile, BossLightningBoltBulletProjectile };

	virtual ~Projectile();

    virtual void Setup(GameModel&) {};
    virtual void Teardown(GameModel&) {};

	virtual void Tick(double seconds, const GameModel& model) = 0;
	virtual BoundingLines BuildBoundingLines() const          = 0;
	virtual ProjectileType GetType() const                    = 0;
    virtual float GetDamage() const                           = 0;
    virtual bool IsRocket() const                             = 0;
    virtual bool IsMine() const                               = 0;
    virtual bool IsRefractableOrReflectable() const           = 0;
    
    virtual float GetZOffset() const { return 0.0f; }

    virtual bool BlastsThroughSafetyNets() const { return true; }
    virtual bool IsDestroyedBySafetyNets() const { return false; }

    virtual void SafetyNetCollisionOccurred(SafetyNet* safetyNet) { this->SetLastThingCollidedWith(safetyNet); };
    virtual void LevelPieceCollisionOccurred(LevelPiece* block)   { UNUSED_PARAMETER(block); };
    virtual void PaddleCollisionOccurred(PlayerPaddle* paddle)    { UNUSED_PARAMETER(paddle); };
    virtual void BossCollisionOccurred(Boss* boss, BossBodyPart* bossPart) { UNUSED_PARAMETER(boss); this->SetLastThingCollidedWith(bossPart); };

    virtual bool ModifyLevelUpdate(double dT, GameModel&) { UNUSED_PARAMETER(dT); return false; };

    virtual void DetachFromPaddle() {};
    virtual bool IsAttachedToSomething() const { return false; };

    void AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec);

    bool GetIsInvisible() const { return this->isInvisible; }
    void SetIsInvisible(bool isInvisible) { this->isInvisible = isInvisible; }

	float GetWidth() const { return this->currWidth; }
	float GetHeight() const { return  this->currHeight; }
	float GetHalfWidth() const { return 0.5f * this->currWidth; }
	float GetHalfHeight() const { return  0.5f * this->currHeight; }
	void SetWidth(float width) { this->currWidth = width; }
	void SetHeight(float height) { this->currHeight = height; }

    // Inherited from the IPositionObject interface
    Point3D GetPosition3D() const {
        return Point3D(this->GetPosition(), 0.0f);
    }

	const Point2D& GetPosition() const { return this->position; }

	const Vector2D& GetVelocityDirection() const { return this->velocityDir; }
	float GetVelocityMagnitude() const { return this->velocityMag; }
    Vector2D GetVelocity() const { return this->GetVelocityMagnitude() * this->GetVelocityDirection(); }

	virtual void SetPosition(const Point2D& pos) { this->position = pos; }

	void SetVelocity(const Vector2D& velocityDir, float velocityMag) { 
		this->velocityDir = velocityDir;
		this->velocityMag = velocityMag;

		// Set the right vector... (short cut which is a 90 degree rotation about the z axis)
		this->rightVec = Vector2D(this->velocityDir[1], -this->velocityDir[0]);
	}
	void SetVelocity(const Vector2D& velocity) { 
		this->velocityMag = Vector2D::Magnitude(velocity);
        if (this->velocityMag < EPSILON) {
            return;
        }
        
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

    static float GetProjectileSplitScaleFactor(int numSplits);

	virtual bool GetIsActive() const {
		return true;
	}

#ifdef _DEBUG
    void DebugDraw() const;
#endif

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

    bool isInvisible;

};

inline float Projectile::GetProjectileSplitScaleFactor(int numSplits) {
    return std::min<float>(1.0f, std::max<float>(0.66666f, (1.5f / static_cast<float>(numSplits))));
}

#endif