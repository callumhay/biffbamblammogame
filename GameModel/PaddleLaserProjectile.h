/**
 * PaddleLaserProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLELASERPROJECTILE_H__
#define __PADDLELASERPROJECTILE_H__

#include "LaserBulletProjectile.h"

class PaddleLaserProjectile : public LaserBulletProjectile {
public:
	static const float HEIGHT_DEFAULT;
	static const float WIDTH_DEFAULT;
    static const float DAMAGE_DEFAULT;

    explicit PaddleLaserProjectile(const Point2D& spawnLoc);
    PaddleLaserProjectile(const PaddleLaserProjectile& copy);
    ~PaddleLaserProjectile();

    ProjectileType GetType() const {
        return Projectile::PaddleLaserBulletProjectile;
    }
    float GetDamage() const { return DAMAGE_DEFAULT; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return true;  }

private:
    // Disallow assignment
    void operator=(const LaserBulletProjectile& copy);
};

#endif // __PADDLELASERPROJECTILE_H__