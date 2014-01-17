/**
 * PaddleFlameBlasterProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleFlameBlasterProjectile.h"
#include "LevelPiece.h"

const float PaddleFlameBlasterProjectile::DEFAULT_VELOCITY_MAG = 8.0f;
const float PaddleFlameBlasterProjectile::DEFAULT_SIZE         = 0.6f * LevelPiece::PIECE_WIDTH;

// Flames start out just above the top of the paddle at a very small size and grow quickly to
// a fixed maximum size
PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PlayerPaddle& firingPaddle) :
PaddleBlasterProjectile(firingPaddle, DEFAULT_SIZE, DEFAULT_VELOCITY_MAG) {
}

PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PaddleFlameBlasterProjectile& copy) :
PaddleBlasterProjectile(copy) {
}

PaddleFlameBlasterProjectile::~PaddleFlameBlasterProjectile() {
}
