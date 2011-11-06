/**
 * BallLaserProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BallLaserProjectile.h"

BallLaserProjectile::BallLaserProjectile(const Point2D& spawnLoc) :
LaserBulletProjectile(spawnLoc, 1, 1, 6.0f, Vector2D(0, 1)) {
}

BallLaserProjectile::BallLaserProjectile(const BallLaserProjectile& copy) :
LaserBulletProjectile(copy) {
}

BallLaserProjectile::~BallLaserProjectile() {
}
