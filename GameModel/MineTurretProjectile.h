/**
 * MineTurretProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MINETURRETPROJECTILE_H__
#define __MINETURRETPROJECTILE_H__

#include "MineProjectile.h"

class MineTurretProjectile : public MineProjectile {

public:
	static const float HEIGHT_DEFAULT;
	static const float WIDTH_DEFAULT;

	MineTurretProjectile(const Point2D& spawnLoc, const Vector2D& velDir);
	MineTurretProjectile(const MineTurretProjectile& copy);
    ~MineTurretProjectile();
    
    ProjectileType GetType() const {
        return Projectile::MineTurretBulletProjectile;
    }

    float GetDamage() const { return 200.0f; }
 
    float GetDefaultHeight() const { return MineTurretProjectile::HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return MineTurretProjectile::WIDTH_DEFAULT;  }
};

#endif // __MINETURRETPROJECTILE_H__