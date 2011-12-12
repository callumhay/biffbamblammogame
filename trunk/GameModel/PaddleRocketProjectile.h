/**
 * PaddleRocketProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEROCKETPROJECTILE_H__
#define __PADDLEROCKETPROJECTILE_H__

#include "RocketProjectile.h"

/**
 * Projectile for the rocket shot from the player paddle when the player acquires a rocket paddle item.
 */
class PaddleRocketProjectile : public RocketProjectile {
public:
	static const float PADDLEROCKET_HEIGHT_DEFAULT;
	static const float PADDLEROCKET_WIDTH_DEFAULT;

	PaddleRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height);
	~PaddleRocketProjectile();
    
    ProjectileType GetType() const {
        return Projectile::PaddleRocketBulletProjectile;
    }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the rocket (1.0) this rocket
        // is currently - used to determine how much it will destroy.
        return (this->GetWidth() / PADDLEROCKET_WIDTH_DEFAULT) * 400.0f;
    }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    float GetAccelerationMagnitude() const { return 2.0f; }
    float GetRotationAccelerationMagnitude() const { return 60.0f; }

    float GetMaxVelocityMagnitude() const { return 20.0f; }
    float GetMaxRotationVelocityMagnitude() const { return 400.0f; }

    float GetDefaultHeight() const { return PADDLEROCKET_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return PADDLEROCKET_WIDTH_DEFAULT;  }

};

#endif // __PADDLEROCKETPROJECTILE_H__