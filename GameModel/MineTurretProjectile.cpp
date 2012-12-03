/**
 * MineTurretProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MineTurretProjectile.h"

const float MineTurretProjectile::HEIGHT_DEFAULT = 0.45f;
const float MineTurretProjectile::WIDTH_DEFAULT  = 0.45f;

MineTurretProjectile::MineTurretProjectile(const Point2D& spawnLoc, const Vector2D& velDir) :
MineProjectile(spawnLoc, velDir, MineTurretProjectile::WIDTH_DEFAULT, MineTurretProjectile::HEIGHT_DEFAULT) {
}

MineTurretProjectile::MineTurretProjectile(const MineTurretProjectile& copy) : MineProjectile(copy) {
}

MineTurretProjectile::~MineTurretProjectile() {
}
