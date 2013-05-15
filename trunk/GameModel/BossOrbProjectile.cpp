/**
 * BossOrbProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossOrbProjectile.h"

const float BossOrbProjectile::RADIUS_DEFAULT = 0.45f;
const float BossOrbProjectile::SPD_DEFAULT    = 13.0f;

BossOrbProjectile::BossOrbProjectile(const Point2D& spawnLoc) :
OrbProjectile(spawnLoc, RADIUS_DEFAULT, SPD_DEFAULT, Vector2D(0, -1)) {
}

BossOrbProjectile::BossOrbProjectile(const Point2D& spawnLoc, const Vector2D& dirVec) :
OrbProjectile(spawnLoc, RADIUS_DEFAULT, SPD_DEFAULT, dirVec) {
}

BossOrbProjectile::BossOrbProjectile(const BossOrbProjectile& copy) : OrbProjectile(copy) {
}

BossOrbProjectile::~BossOrbProjectile() {
}