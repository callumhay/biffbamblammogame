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
#include "PlayerPaddle.h"
#include "LevelPiece.h"

const float PaddleFlameBlasterProjectile::DIST_FROM_TOP_OF_PADDLE_TO_FLAME = 1.0f;
const float PaddleFlameBlasterProjectile::DEFAULT_VELOCITY_MAG             = 8.0f;

// Flames start out just above the top of the paddle at a very small size and grow quickly to
// a fixed maximum size
PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PlayerPaddle& firingPaddle) :
Projectile(firingPaddle.GetCenterPosition() + Vector2D(0, firingPaddle.GetHalfHeight() + DIST_FROM_TOP_OF_PADDLE_TO_FLAME), 1, 1) {
    
    float size = 0.6f * LevelPiece::PIECE_WIDTH * firingPaddle.GetPaddleScaleFactor();
    this->SetWidth(size);
    this->SetHeight(size);

    this->SetVelocity(firingPaddle.GetUpVector(), PaddleFlameBlasterProjectile::DEFAULT_VELOCITY_MAG);
}

PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PaddleFlameBlasterProjectile& copy) :
Projectile(copy) {
}

PaddleFlameBlasterProjectile::~PaddleFlameBlasterProjectile() {
}

BoundingLines PaddleFlameBlasterProjectile::BuildBoundingLines() const {

    Point2D topRight    = this->GetPosition() + this->GetHalfHeight()*this->velocityDir + this->GetHalfWidth()*this->rightVec;
    Point2D bottomRight = topRight - this->GetHeight()*this->velocityDir;
    Point2D topLeft     = topRight - this->GetWidth()*this->rightVec;
    Point2D bottomLeft  = topLeft - this->GetHeight()*this->velocityDir;

    static const int NUM_BOUNDING_LINES = 4;
    
    Collision::LineSeg2D bounds[NUM_BOUNDING_LINES];
    Vector2D norms[NUM_BOUNDING_LINES];
    
    bounds[0] = Collision::LineSeg2D(topLeft, bottomLeft);
    bounds[1] = Collision::LineSeg2D(topRight, bottomRight);
    bounds[2] = Collision::LineSeg2D(topRight, topLeft);
    bounds[3] = Collision::LineSeg2D(bottomRight, bottomLeft);

    return BoundingLines(NUM_BOUNDING_LINES, bounds, norms);
}
