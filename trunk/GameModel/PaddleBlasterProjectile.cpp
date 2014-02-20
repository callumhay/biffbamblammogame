/**
 * PaddleBlasterProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleBlasterProjectile.h"
#include "PlayerPaddle.h"
#include "CannonBlock.h"
#include "GameEventManager.h"

const float PaddleBlasterProjectile::DEFAULT_DIST_FROM_TOP_OF_PADDLE_TO_PROJECTILE = 0.85f;

// Flames start out just above the top of the paddle at a very small size and grow quickly to
// a fixed maximum size
PaddleBlasterProjectile::PaddleBlasterProjectile(const PlayerPaddle& firingPaddle, 
                                                 float defaultSize, float defaultVelMag) :
Projectile(firingPaddle.GetCenterPosition() + 
           Vector2D(0, firingPaddle.GetHalfHeight() + 
           PaddleBlasterProjectile::GetOriginDistanceFromTopOfPaddle(firingPaddle)), 1, 1),
cannonBlock(NULL), defaultVelocityMagnitude(defaultVelMag) {

    float size = defaultSize * firingPaddle.GetPaddleScaleFactor();
    this->SetWidth(size);
    this->SetHeight(size);

    this->SetVelocity(firingPaddle.GetUpVector(), defaultVelMag);
}

PaddleBlasterProjectile::PaddleBlasterProjectile(const PaddleBlasterProjectile& copy) :
Projectile(copy), cannonBlock(copy.cannonBlock), defaultVelocityMagnitude(copy.defaultVelocityMagnitude) {
}

PaddleBlasterProjectile::~PaddleBlasterProjectile() {
    this->cannonBlock = NULL;
}

void PaddleBlasterProjectile::Tick(double seconds, const GameModel& model) {
    if (this->cannonBlock == NULL) {
        this->AugmentDirectionOnPaddleMagnet(seconds, model, 40.0f);

        // Update the laser's position
        this->position = this->position + (seconds * this->velocityMag * this->velocityDir);
    }
}

bool PaddleBlasterProjectile::ModifyLevelUpdate(double dT, GameModel& model) {
    if (this->cannonBlock != NULL) {

        // Start by doing a test to see if the cannon will actually fire...
        bool willCannonFire = this->cannonBlock->TestRotateAndFire(dT, false, this->velocityDir);
        if (willCannonFire) {
            // Set the remaining kinematics properties of the projectile so that it's fired
            this->velocityMag = this->defaultVelocityMagnitude;
            this->position    = this->cannonBlock->GetCenter() + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

            // EVENT: This blast has officially been fired from the cannon
            GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);
        }

        // 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
        // it has fired the rocket
        // WARNING: The cannon block may no longer exist after this function call!
        std::pair<LevelPiece*, bool> cannonFiredPair = this->cannonBlock->RotateAndFire(dT, &model, false);
        if (willCannonFire) {
            assert(cannonFiredPair.second);
            this->cannonBlock = NULL;
            this->SetLastThingCollidedWith(cannonFiredPair.first);
        }
    }

    return false;
}

BoundingLines PaddleBlasterProjectile::BuildBoundingLines() const {

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

void PaddleBlasterProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
    assert(cannonBlock != NULL);

    // When loaded into a cannon block the rocket stops moving and centers on the block...
    this->velocityMag  = 0.0f;
    this->position    = cannonBlock->GetCenter();
    this->cannonBlock = cannonBlock;

    // EVENT: The blast is officially loaded into the cannon block
    GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}

float PaddleBlasterProjectile::GetOriginDistanceFromTopOfPaddle(const PlayerPaddle& firingPaddle) {
    return firingPaddle.GetPaddleScaleFactor()* DEFAULT_DIST_FROM_TOP_OF_PADDLE_TO_PROJECTILE;
}