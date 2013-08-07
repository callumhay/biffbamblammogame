/**
 * PaddleLaserProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleLaserProjectile.h"

const float PaddleLaserProjectile::HEIGHT_DEFAULT	= 1.2f;  // Height of a laser projectile in game units
const float PaddleLaserProjectile::WIDTH_DEFAULT	= 0.5f;  // Width of a laser projectile in game units 
const float PaddleLaserProjectile::DAMAGE_DEFAULT   = 50.0f; // Damage the laser projectile does by default

PaddleLaserProjectile::PaddleLaserProjectile(const Point2D& spawnLoc, const Vector2D& laserVelDir) :
LaserBulletProjectile(spawnLoc, WIDTH_DEFAULT, HEIGHT_DEFAULT, 10.0f, laserVelDir) { 
}
PaddleLaserProjectile::PaddleLaserProjectile(const PaddleLaserProjectile& copy) :
LaserBulletProjectile(copy) {
}
PaddleLaserProjectile::~PaddleLaserProjectile() {
}
