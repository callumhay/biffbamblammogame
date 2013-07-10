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

// Amount of time that collisions are disabled between this rocket and the player paddle 
// (to accommodate the time it takes for the rocket to take-off away from the paddle).
const double PaddleRemoteControlRocketProjectile::PADDLE_COLLISIONS_DISABLED_TIME_IN_SECS = 1.5;

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(
    const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height) :
RocketProjectile(spawnLoc, rocketVelDir, width, height), 
paddleCollisionsDisabledTimer(PaddleRemoteControlRocketProjectile::PADDLE_COLLISIONS_DISABLED_TIME_IN_SECS) {
}

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(const PaddleRemoteControlRocketProjectile& copy) : 
RocketProjectile(copy), paddleCollisionsDisabledTimer(copy.paddleCollisionsDisabledTimer) {
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
}

void PaddleRemoteControlRocketProjectile::Tick(double seconds, const GameModel& model) {
    RocketProjectile::Tick(seconds, model);

    // Tick down the paddle collisions disabled timer (this is so the rocket doesn't collide with
    // the paddle when it's initially taking off)
    this->paddleCollisionsDisabledTimer = std::max<double>(0.0, this->paddleCollisionsDisabledTimer - seconds);


}