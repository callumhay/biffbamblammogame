/**
 * BossRocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSROCKETPROJECTILE__H__
#define __BOSSROCKETPROJECTILE__H__

#include "RocketProjectile.h"
#include "RocketTurretProjectile.h"

class BossRocketProjectile : public RocketProjectile {
public:
	static const float BOSS_ROCKET_HEIGHT_DEFAULT;
	static const float BOSS_ROCKET_WIDTH_DEFAULT;

	BossRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float zOffset);
	~BossRocketProjectile();
    
    ProjectileType GetType() const { return Projectile::BossRocketBulletProjectile; }

    float GetDamage() const { return 100.0f; }

    float GetZOffset() const { return this->zOffset; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }

    float GetAccelerationMagnitude() const { return 13.0f; }
    float GetRotationAccelerationMagnitude() const { return 300.0f; }

    float GetMaxVelocityMagnitude() const { return 18.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return RocketTurretProjectile::TURRETROCKET_WIDTH_DEFAULT;  }

private:
    float zOffset;
};

#endif // __BOSSROCKETPROJECTILE__H__