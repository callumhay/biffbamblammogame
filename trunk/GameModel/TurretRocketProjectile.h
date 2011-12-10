/**
 * TurretRocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TURRETROCKETPROJECTILE_H__
#define __TURRETROCKETPROJECTILE_H__

#include "RocketProjectile.h"

class TurretRocketProjectile : public RocketProjectile {
public:
	static const float TURRETROCKET_HEIGHT_DEFAULT;
	static const float TURRETROCKET_WIDTH_DEFAULT;

	TurretRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir);
	~TurretRocketProjectile();
    
    ProjectileType GetType() const { return Projectile::RocketTurretBulletProjectile; }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the rocket (1.0) this rocket
        // is currently - used to determine how much it will destroy.
        return (this->GetWidth() / TURRETROCKET_WIDTH_DEFAULT) * 100.0f;
    }

    float GetAccelerationMagnitude() const { return 5.0f; }
    float GetRotationAccelerationMagnitude() const { return 100.0f; }

    float GetMaxVelocityMagnitude() const { return 20.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return TURRETROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return TURRETROCKET_WIDTH_DEFAULT;  }
};

#endif // __TURRETROCKETPROJECTILE_H__