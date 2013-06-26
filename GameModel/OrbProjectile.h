/**
 * OrbProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ORBPROJECTILE_H__
#define __ORBPROJECTILE_H__

#include "Projectile.h"

/**
 * Represents a circle shaped projectile.
 */
class OrbProjectile : public Projectile {
public:
	virtual ~OrbProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

protected:
    OrbProjectile(const Point2D& spawnLoc, float radius, float velocityMag, const Vector2D& velocityDir);
	OrbProjectile(const OrbProjectile& copy);

private:
    // Disallow assignment
    void operator=(const OrbProjectile& copy);
};

#endif // __ORBPROJECTILE_H__