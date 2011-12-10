/**
 * PaddleRocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleRocketProjectile.h"

const float PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT = 1.50f;
const float PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT  = 0.8f;

PaddleRocketProjectile::PaddleRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir,
                                               float width, float height) :
RocketProjectile(spawnLoc, rocketVelDir, width, height) {
}

PaddleRocketProjectile::~PaddleRocketProjectile() {
}
