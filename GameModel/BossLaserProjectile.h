/**
 * BossLaserProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSLASERPROJECTILE_H__
#define __BOSSLASERPROJECTILE_H__

#include "LaserBulletProjectile.h"

class BossLaserProjectile : public LaserBulletProjectile {
public:
	static const float HEIGHT_DEFAULT;
	static const float WIDTH_DEFAULT;
    static const float SPD_DEFAULT;

    explicit BossLaserProjectile(const Point2D& spawnLoc);
    BossLaserProjectile(const Point2D& spawnLoc, const Vector2D& dirVec);
    //BossLaserProjectile(const Point2D& spawnLoc, const Vector2D& dirVec, float width);
    BossLaserProjectile(const BossLaserProjectile& copy);
    ~BossLaserProjectile();

    ProjectileType GetType() const {
        return Projectile::BossLaserBulletProjectile;
    }
    float GetDamage() const { return 0.0f; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return true;  }

private:
    // Disallow assignment
    void operator=(const BossLaserProjectile& copy);
};

#endif // __BOSSLASERPROJECTILE_H__