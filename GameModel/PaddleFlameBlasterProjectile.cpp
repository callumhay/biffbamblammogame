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
#include "CannonBlock.h"
#include "GameEventManager.h"

const float PaddleFlameBlasterProjectile::DIST_FROM_TOP_OF_PADDLE_TO_FLAME = 1.0f;
const float PaddleFlameBlasterProjectile::DEFAULT_VELOCITY_MAG             = 8.0f;
const float PaddleFlameBlasterProjectile::DEFAULT_SIZE                     = 0.6f * LevelPiece::PIECE_WIDTH;

// Flames start out just above the top of the paddle at a very small size and grow quickly to
// a fixed maximum size
PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PlayerPaddle& firingPaddle) :
Projectile(firingPaddle.GetCenterPosition() + Vector2D(0, firingPaddle.GetHalfHeight() + DIST_FROM_TOP_OF_PADDLE_TO_FLAME), 1, 1),
cannonBlock(NULL) {
    
    float size = DEFAULT_SIZE * firingPaddle.GetPaddleScaleFactor();
    this->SetWidth(size);
    this->SetHeight(size);

    this->SetVelocity(firingPaddle.GetUpVector(), PaddleFlameBlasterProjectile::DEFAULT_VELOCITY_MAG);
}

PaddleFlameBlasterProjectile::PaddleFlameBlasterProjectile(const PaddleFlameBlasterProjectile& copy) :
Projectile(copy), cannonBlock(copy.cannonBlock) {
}

PaddleFlameBlasterProjectile::~PaddleFlameBlasterProjectile() {
    this->cannonBlock = NULL;
}

void PaddleFlameBlasterProjectile::Tick(double seconds, const GameModel& model) {
    if (this->cannonBlock != NULL) {
        // 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
        // it has fired the rocket
        bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);
        if (cannonHasFired) {
            // Set the velocity in the direction the cannon has fired in
            this->velocityMag = DEFAULT_VELOCITY_MAG;
            this->velocityDir = this->cannonBlock->GetCurrentCannonDirection();
            this->position = this->cannonBlock->GetCenter() + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

            // EVENT: This blast has officially been fired from the cannon
            GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);

            this->SetLastThingCollidedWith(this->cannonBlock);
            this->cannonBlock = NULL;
        }
    }
    else {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 40.0f);

        // Update the laser's position
        this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
    }
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

void PaddleFlameBlasterProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
    assert(cannonBlock != NULL);

    // When loaded into a cannon block the rocket stops moving and centers on the block...
    this->velocityMag  = 0.0f;
    this->position    = cannonBlock->GetCenter();
    this->cannonBlock = cannonBlock;

    // EVENT: The blast is officially loaded into the cannon block
    GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}