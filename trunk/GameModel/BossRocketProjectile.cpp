/**
 * BossRocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossRocketProjectile.h"

const float BossRocketProjectile::BOSS_ROCKET_WIDTH_DEFAULT  = 0.75f;
const float BossRocketProjectile::BOSS_ROCKET_HEIGHT_DEFAULT = 1.4f;

BossRocketProjectile::BossRocketProjectile(const Point2D& spawnLoc, const Vector2D& rocketVelDir, float zOffset) :
RocketProjectile(spawnLoc, rocketVelDir, BOSS_ROCKET_WIDTH_DEFAULT, BOSS_ROCKET_HEIGHT_DEFAULT), zOffset(zOffset) {
}

BossRocketProjectile::~BossRocketProjectile() {
}
