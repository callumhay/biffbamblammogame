/**
 * PaddleMineProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEMINEPROJECTILE_H__
#define __PADDLEMINEPROJECTILE_H__

#include "Projectile.h"

class CannonBlock;

/**
 * Projectile for a mine launched from the player paddle when the player acquires a mine launcher paddle item.
 */
class PaddleMineProjectile : public Projectile {
public:
	static const float PADDLEMINE_HEIGHT_DEFAULT;
	static const float PADDLEMINE_WIDTH_DEFAULT;

	PaddleMineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height);
	PaddleMineProjectile(const PaddleMineProjectile& copy);
    ~PaddleMineProjectile();
    
	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

    ProjectileType GetType() const {
        return Projectile::PaddleMineBulletProjectile;
    }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the mine (1.0),
        // this is used to determine how much it will destroy.
        return (this->GetWidth() / this->GetDefaultWidth()) * 200.0f;
    }

    bool IsRocket() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

	void LoadIntoCannonBlock(CannonBlock* cannonBlock);
    bool IsLoadedInCannonBlock() const { return this->cannonBlock != NULL; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    float GetAccelerationMagnitude() const { return 100.0f; }
    float GetRotationAccelerationMagnitude() const { return 360.0f; }

    float GetMaxVelocityMagnitude() const { return 20.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return PADDLEMINE_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return PADDLEMINE_WIDTH_DEFAULT;  }

private:
	static const Vector2D MINE_DEFAULT_VELOCITYDIR;
	static const Vector2D MINE_DEFAULT_RIGHTDIR;

    // When the mine is loaded into a cannon block this will not be NULL
	CannonBlock* cannonBlock;

	// Rotation of the mine as it moves
	float currRotationSpd;
	float currRotation;
};

#endif // __PADDLEMINEPROJECTILE_H__