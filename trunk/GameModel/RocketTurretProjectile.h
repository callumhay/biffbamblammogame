/**
 * RocketTurretProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __TURRETROCKETPROJECTILE_H__
#define __TURRETROCKETPROJECTILE_H__

#include "RocketProjectile.h"
#include "RocketTurretBlock.h"

class RocketTurretProjectile : public RocketProjectile {
public:
	static const float TURRETROCKET_HEIGHT_DEFAULT;
	static const float TURRETROCKET_WIDTH_DEFAULT;

	RocketTurretProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir);
	~RocketTurretProjectile();
    
    ProjectileType GetType() const { return Projectile::RocketTurretBulletProjectile; }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the rocket (1.0) this rocket
        // is currently - used to determine how much it will destroy.
        return (this->GetWidth() / TURRETROCKET_WIDTH_DEFAULT) * 100.0f;
    }

    float GetZOffset() const { return RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }

    float GetAccelerationMagnitude() const { return 7.0f; }
    float GetRotationAccelerationMagnitude() const { return 100.0f; }

    float GetMaxVelocityMagnitude() const { return 20.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return TURRETROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return TURRETROCKET_WIDTH_DEFAULT;  }
};

#endif // __TURRETROCKETPROJECTILE_H__