/**
 * BossOrbProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSORBPROJECTILE_H__
#define __BOSSORBPROJECTILE_H__

#include "OrbProjectile.h"

/**
 * Represents orb projectiles that are shot by bosses.
 */
class BossOrbProjectile : public OrbProjectile {
public:
	static const float RADIUS_DEFAULT;
    static const float SPD_DEFAULT;

    explicit BossOrbProjectile(const Point2D& spawnLoc);
    BossOrbProjectile(const Point2D& spawnLoc, const Vector2D& dirVec);
    BossOrbProjectile(const BossOrbProjectile& copy);
    ~BossOrbProjectile();

    ProjectileType GetType() const {
        return Projectile::BossOrbBulletProjectile;
    }

    float GetDamage() const { return 0.0f; }

    bool IsRefractableOrReflectable() const { return true; }
    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return true;  }

private:
    // Disallow assignment
    void operator=(const BossOrbProjectile& copy);
};

#endif // __BOSSORBPROJECTILE_H__