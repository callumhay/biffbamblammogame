/**
 * RocketTurretProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RocketTurretProjectile.h"
#include "RocketTurretBlock.h"

const float RocketTurretProjectile::TURRETROCKET_WIDTH_DEFAULT  = 0.41f;
const float RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT = 0.72f;

RocketTurretProjectile::RocketTurretProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir) :
RocketProjectile(spawnLoc, rocketVelDir, TURRETROCKET_WIDTH_DEFAULT, TURRETROCKET_HEIGHT_DEFAULT) {
}

RocketTurretProjectile::~RocketTurretProjectile() {
}
