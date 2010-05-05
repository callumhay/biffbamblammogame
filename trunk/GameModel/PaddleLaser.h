/**
 * PaddleLaser.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLELASER_H__
#define __PADDLELASER_H__

#include "Projectile.h"

/**
 * Projectile representing laser shots/bullets fired by the player paddle.
 */
class PaddleLaser : public Projectile {
private:
	static const Vector2D PADDLELASER_VELOCITYDIR;
	static const Vector2D PADDLELASER_RIGHTDIR;
	static const float PADDLELASER_VELOCITYMAG;

public:
	static const float PADDLELASER_HEIGHT_DEFAULT;
	static const float PADDLELASER_WIDTH_DEFAULT;

	PaddleLaser(const Point2D& spawnLoc);
	PaddleLaser(const PaddleLaser& copy);
	PaddleLaser(const Projectile& copy);
	~PaddleLaser();

	void Tick(double seconds);
	BoundingLines BuildBoundingLines() const;

};

#endif // __PADDLELASER_H__