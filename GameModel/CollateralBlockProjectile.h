/**
 * CollateralBlockProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __COLLATERALBLOCKPROJECTILE_H__
#define __COLLATERALBLOCKPROJECTILE_H__

#include "Projectile.h"

class CollateralBlock;

/**
 * A projectile that holds a collateral block during its warning and
 * collateral damage state.
 */
class CollateralBlockProjectile : public Projectile {
public:
	CollateralBlockProjectile(CollateralBlock* collateralBlock);
	~CollateralBlockProjectile();

    bool IsRocket() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;
    ProjectileType GetType() const {
        return Projectile::CollateralBlockProjectile;
    }
    float GetDamage() const { return 9999.99f; }

    void SetPosition(const Point2D& pos);

private:
	CollateralBlock* collateralBlock;

	DISALLOW_COPY_AND_ASSIGN(CollateralBlockProjectile);
};

#endif // __COLLATERALBLOCKPROJECTILE_H__