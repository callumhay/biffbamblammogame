/**
 * CollateralBlockProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
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

	void Tick(double seconds);
	BoundingLines BuildBoundingLines() const;

private:
	CollateralBlock* collateralBlock;

};

#endif // __COLLATERALBLOCKPROJECTILE_H__