/**
 * FireGlobProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __FIREGLOBPROJECTILE_H__
#define __FIREGLOBPROJECTILE_H__

#include "Projectile.h"

class FireGlobProjectile : public Projectile {
public:
	FireGlobProjectile(const Point2D& spawnLoc, float width, float height);
	~FireGlobProjectile();

	void Tick(double seconds);
	BoundingLines BuildBoundingLines() const;

private:
	static const float FIRE_GLOB_MIN_VELOCITY;
	static const float FIRE_GLOB_MAX_ADD_VELOCITY;

	float xMovementVariation;		// The variation of the x-axis movement as the fire glob falls
	double totalTickTime;				// The total time ticked on this projectile so far

	DISALLOW_COPY_AND_ASSIGN(FireGlobProjectile);
};

#endif // __FIREGLOBPROJECTILE_H__