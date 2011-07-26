/**
 * BallLaserProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLLASERPROJECTILE_H__
#define __BALLLASERPROJECTILE_H__

#include "LaserBulletProjectile.h"

class BallLaserProjectile : public LaserBulletProjectile {
public:
    BallLaserProjectile(const Point2D& spawnLoc);
    BallLaserProjectile(const BallLaserProjectile& copy);
    ~BallLaserProjectile();

    ProjectileType GetType() const {
        return Projectile::BallLaserBulletProjectile;
    }

private:
    // Disallow assignment
    void operator=(const BallLaserProjectile& copy);
};

#endif // __BALLLASERPROJECTILE_H__