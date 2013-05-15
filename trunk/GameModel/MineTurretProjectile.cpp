/**
 * MineTurretProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MineTurretProjectile.h"

const float MineTurretProjectile::TURRET_MINE_HEIGHT = 0.65f;
const float MineTurretProjectile::TURRET_MINE_WIDTH  = 0.65f;

MineTurretProjectile::MineTurretProjectile(const Point2D& spawnLoc, const Vector2D& velDir) :
MineProjectile(spawnLoc, velDir, MineTurretProjectile::TURRET_MINE_WIDTH, MineTurretProjectile::TURRET_MINE_HEIGHT) {
    this->velocityMag = MineProjectile::MAX_VELOCITY;
}

MineTurretProjectile::MineTurretProjectile(const MineTurretProjectile& copy) : MineProjectile(copy) {
}

MineTurretProjectile::~MineTurretProjectile() {
}
