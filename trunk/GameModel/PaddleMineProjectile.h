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

#include "MineProjectile.h"

/**
 * Projectile for a mine launched from the player paddle when the player acquires a mine launcher paddle item.
 */
class PaddleMineProjectile : public MineProjectile {

public:
	static const float PADDLEMINE_HEIGHT_DEFAULT;
	static const float PADDLEMINE_WIDTH_DEFAULT;

	PaddleMineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height);
	PaddleMineProjectile(const PaddleMineProjectile& copy);
    ~PaddleMineProjectile();
    
    ProjectileType GetType() const {
        return Projectile::PaddleMineBulletProjectile;
    }

    float GetDamage() const {
        // Obtain the size relative to the the normal size of the mine (1.0),
        // this is used to determine how much it will destroy.
        return (this->GetWidth() / this->GetDefaultWidth()) * 200.0f;
    }
 
    float GetDefaultHeight() const { return PaddleMineProjectile::PADDLEMINE_HEIGHT_DEFAULT; }
    float GetDefaultWidth() const  { return PaddleMineProjectile::PADDLEMINE_WIDTH_DEFAULT;  }
};

#endif // __PADDLEMINEPROJECTILE_H__