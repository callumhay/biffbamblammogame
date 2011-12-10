/**
 * TurretRocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TurretRocketProjectile.h"
#include "RocketTurretBlock.h"

const float TurretRocketProjectile::TURRETROCKET_WIDTH_DEFAULT  = 1.99f * RocketTurretBlock::ROCKET_HOLE_RADIUS;
const float TurretRocketProjectile::TURRETROCKET_HEIGHT_DEFAULT = 1.9f * TURRETROCKET_WIDTH_DEFAULT;

TurretRocketProjectile::TurretRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir) :
RocketProjectile(spawnLoc, rocketVelDir, TURRETROCKET_WIDTH_DEFAULT, TURRETROCKET_HEIGHT_DEFAULT) {
}

TurretRocketProjectile::~TurretRocketProjectile() {
}
