/**
 * PaddleMineProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleMineProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"
#include "LevelPiece.h"
#include "GameModel.h"

PaddleMineProjectile::PaddleMineProjectile(const Point2D& spawnLoc, const Vector2D& velDir,
                                           float width, float height) :
MineProjectile(spawnLoc, velDir, width, height) {
}

PaddleMineProjectile::PaddleMineProjectile(const PaddleMineProjectile& copy) : MineProjectile(copy) {
}

PaddleMineProjectile::~PaddleMineProjectile() {
}
