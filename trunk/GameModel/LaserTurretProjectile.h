/**
 * LaserTurretProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERTURRETPROJECTILE_H__
#define __LASERTURRETPROJECTILE_H__

#include "LaserBulletProjectile.h"

class LaserTurretProjectile : public LaserBulletProjectile {
public:
	static const float HEIGHT_DEFAULT;
	static const float WIDTH_DEFAULT;

    LaserTurretProjectile(const Point2D& spawnLoc, const Vector2D& dir);
    LaserTurretProjectile(const LaserTurretProjectile& copy);
    ~LaserTurretProjectile();

    ProjectileType GetType() const {
        return Projectile::LaserTurretBulletProjectile;
    }

    float GetDamage() const { return 80.0f; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return true;  }

private:
    // Disallow assignment
    void operator=(const LaserTurretProjectile& copy);
};

#endif // __LASERTURRETPROJECTILE_H__