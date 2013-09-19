/**
 * BossLightningBoltProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSLIGHTNINGBOLTPROJECTILE_H__
#define __BOSSLIGHTNINGBOLTPROJECTILE_H__

#include "Projectile.h"

class BossLightningBoltProjectile : public Projectile {
public:
    static const float HEIGHT_DEFAULT;
    static const float WIDTH_DEFAULT;
    static const float SPD_DEFAULT;

    BossLightningBoltProjectile(const Point2D& spawnLoc, const Vector2D& dirVec, float speed);
    BossLightningBoltProjectile(const BossLightningBoltProjectile& copy);
	~BossLightningBoltProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

    ProjectileType GetType() const {
        return Projectile::BossLightningBoltBulletProjectile;
    }

    float GetDamage() const { return 0.0f; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return true;  }

private:
    // Disallow assignment
    void operator=(const BossLightningBoltProjectile& copy);
};


#endif // __BOSSLIGHTNINGBOLTPROJECTILE_H__