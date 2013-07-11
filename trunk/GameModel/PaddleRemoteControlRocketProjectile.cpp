/**
 * PaddleRemoteControlRocketProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "PaddleRemoteControlRocketProjectile.h"
#include "GameModel.h"

const float PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_HEIGHT_DEFAULT = 1.5f;
const float PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT  = 0.69f;

const float PaddleRemoteControlRocketProjectile::MAX_APPLIED_ACCELERATION       = 9.0f; 
const float PaddleRemoteControlRocketProjectile::DECELLERATION_OF_APPLIED_ACCEL = 16.0f;

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(
    const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height) :
RocketProjectile(spawnLoc, rocketVelDir, width, height), currAppliedAccelDir(0.0f, 0.0f), currAppliedAccelMag(0.0f) {
}

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(const PaddleRemoteControlRocketProjectile& copy) : 
RocketProjectile(copy), currAppliedAccelDir(copy.currAppliedAccelDir), currAppliedAccelMag(copy.currAppliedAccelMag) {
}

PaddleRemoteControlRocketProjectile::~PaddleRemoteControlRocketProjectile() {
}

/// <summary>
/// Called by the GameModel when first setting up this projectile. Use this
/// function to manipulate the game state so that everything freezes and the player gets
/// control over this rocket.
/// </summary>
void PaddleRemoteControlRocketProjectile::Setup(GameModel& gameModel) {
    // While the remote control rocket is activated, the player gains control of
    // it while the paddle and ball are frozen and removed from the game temporarily
    gameModel.SetPause(GameModel::PausePaddle);
    gameModel.SetPause(GameModel::PauseBall);
    gameModel.SetPause(GameModel::PauseTimers);
    
    GameTransformMgr* transformMgr = gameModel.GetTransformInfo();
    assert(transformMgr != NULL);
    transformMgr->SetRemoteControlRocketCamera(true, this);
}

/// <summary>
/// Called by the GameModel when this projectile is just about to be deleted.
/// Use this function to clean up the game state from when it was frozen for the remote control take-over.
/// </summary>
void PaddleRemoteControlRocketProjectile::Teardown(GameModel& gameModel) {
    // Re-enable the paddle and balls
    gameModel.UnsetPause(GameModel::PausePaddle);
    gameModel.UnsetPause(GameModel::PauseBall);
    gameModel.UnsetPause(GameModel::PauseTimers);

    GameTransformMgr* transformMgr = gameModel.GetTransformInfo();
    assert(transformMgr != NULL);
    transformMgr->SetRemoteControlRocketCamera(false, NULL);

    // If the rocket went out of bounds then blow it up...
    if (gameModel.IsOutOfGameBounds(this->GetPosition())) {
        GameLevel* currLevel = gameModel.GetCurrentLevel();
        currLevel->RocketExplosionNoPieces(this);
    }
}

void PaddleRemoteControlRocketProjectile::Tick(double seconds, const GameModel& model) {
    
    // Update the acceleration/velocity/position of the rocket based on the current steering
    if (this->GetVelocityMagnitude() >= 0.5f * this->GetMaxVelocityMagnitude()) {
        this->SetVelocity(this->GetVelocity() + seconds * this->GetAppliedAcceleration());
    }
    
    double dA = DECELLERATION_OF_APPLIED_ACCEL * seconds;
    this->currAppliedAccelMag -= dA;
    if (this->currAppliedAccelMag <= 0.0f) {
        this->currAppliedAccelMag = 0.0f;
        this->currAppliedAccelDir = Vector2D(0,0);
    }

    RocketProjectile::Tick(seconds, model);
}

