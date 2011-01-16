#include "PaddleLaserProjectile.h"

const float PaddleLaserProjectile::HEIGHT_DEFAULT	= 1.2f;             // Height of a laser projectile in game units
const float PaddleLaserProjectile::WIDTH_DEFAULT	= 0.5f;             // Width of a laser projectile in game units 

PaddleLaserProjectile::PaddleLaserProjectile(const Point2D& spawnLoc) :
LaserBulletProjectile(spawnLoc, 0.5f, 1.2f, 10.0f, Vector2D(0, 1)) { 
}
PaddleLaserProjectile::PaddleLaserProjectile(const PaddleLaserProjectile& copy) :
LaserBulletProjectile(copy) {
}
PaddleLaserProjectile::~PaddleLaserProjectile() {
}
