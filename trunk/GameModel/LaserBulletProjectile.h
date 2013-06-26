/**
 * LaserBulletProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LaserBulletProjectile_H__
#define __LaserBulletProjectile_H__

#include "Projectile.h"

/**
 * Projectile representing laser shots/bullets fired by the player paddle.
 */
class LaserBulletProjectile : public Projectile {
public:
	virtual ~LaserBulletProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return true; }

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

protected:
    LaserBulletProjectile(const Point2D& spawnLoc, float width, float height, 
                          float velocityMag, const Vector2D& velocityDir);
	LaserBulletProjectile(const LaserBulletProjectile& copy);

private:
    // Disallow assignment
    void operator=(const LaserBulletProjectile& copy);
};

#endif // __LaserBulletProjectile_H__