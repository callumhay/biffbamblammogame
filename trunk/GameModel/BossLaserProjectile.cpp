/**
 * BossLaserProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossLaserProjectile.h"

const float BossLaserProjectile::HEIGHT_DEFAULT	= 1.2f;  // Height of a laser projectile in game units
const float BossLaserProjectile::WIDTH_DEFAULT	= 0.5f;  // Width of a laser projectile in game units 
const float BossLaserProjectile::SPD_DEFAULT = 13.0f;

BossLaserProjectile::BossLaserProjectile(const Point2D& spawnLoc) :
LaserBulletProjectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT, SPD_DEFAULT, Vector2D(0, -1)) { 
}

BossLaserProjectile::BossLaserProjectile(const Point2D& spawnLoc, const Vector2D& dirVec) :
LaserBulletProjectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT, SPD_DEFAULT, dirVec) { 
}

BossLaserProjectile::BossLaserProjectile(const BossLaserProjectile& copy) :
LaserBulletProjectile(copy) {
}

BossLaserProjectile::~BossLaserProjectile() {
}
