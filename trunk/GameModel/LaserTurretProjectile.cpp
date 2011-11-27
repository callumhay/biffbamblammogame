/**
 * LaserTurretProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserTurretProjectile.h"
#include "LaserTurretBlock.h"

const float LaserTurretProjectile::WIDTH_DEFAULT	= 0.3f;                                         // Width of a laser projectile in game units 
const float LaserTurretProjectile::HEIGHT_DEFAULT	= 2.1f * LaserTurretProjectile::WIDTH_DEFAULT;  // Height of a laser projectile in game units

LaserTurretProjectile::LaserTurretProjectile(const Point2D& spawnLoc, const Vector2D& dir) :
LaserBulletProjectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT, 10.0f, dir) { 
}
LaserTurretProjectile::LaserTurretProjectile(const LaserTurretProjectile& copy) :
LaserBulletProjectile(copy) {
}
LaserTurretProjectile::~LaserTurretProjectile() {
}
