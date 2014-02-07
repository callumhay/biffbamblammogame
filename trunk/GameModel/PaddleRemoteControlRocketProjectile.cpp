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
#include "GameTransformMgr.h"
#include "PortalBlock.h"

const float PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_HEIGHT_DEFAULT = 1.5f;
const float PaddleRemoteControlRocketProjectile::PADDLE_REMOTE_CONTROL_ROCKET_WIDTH_DEFAULT  = 0.69f;

const double PaddleRemoteControlRocketProjectile::TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS = 20.0;

const float PaddleRemoteControlRocketProjectile::MAX_VELOCITY_BEFORE_THRUST = 4.5f;
const float PaddleRemoteControlRocketProjectile::MAX_VELOCITY_WITH_THRUST   = 14.0f;

const float PaddleRemoteControlRocketProjectile::MAX_APPLIED_ACCELERATION       = 10.0f; 
const float PaddleRemoteControlRocketProjectile::DECELLERATION_OF_APPLIED_ACCEL = 16.0f;

const float PaddleRemoteControlRocketProjectile::MAX_APPLIED_THRUST        = 50.0f; 
const float PaddleRemoteControlRocketProjectile::THRUST_DECREASE_RATE      = 200.0f;
const double PaddleRemoteControlRocketProjectile::MIN_TIME_BETWEEN_THRUSTS = 1.0;

const float PaddleRemoteControlRocketProjectile::STARTING_FUEL_AMOUNT          = 100.0f;
const double PaddleRemoteControlRocketProjectile::RATE_OF_FUEL_CONSUMPTION     = STARTING_FUEL_AMOUNT / TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS;
const float PaddleRemoteControlRocketProjectile::FUEL_AMOUNT_TO_START_FLASHING = 33.0f;

const double PaddleRemoteControlRocketProjectile::PORTAL_COLLISION_RESET_TIME = LevelPiece::HALF_PIECE_WIDTH;

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(
    const Point2D& spawnLoc, const Vector2D& rocketVelDir, float width, float height) :
RocketProjectile(spawnLoc, rocketVelDir, width, height), currAppliedAccelDir(0.0f, 0.0f), currAppliedAccelMag(0.0f),
currAppliedThrust(0.0f), currFuelAmt(STARTING_FUEL_AMOUNT), currFlashColourAmt(0.0f), currFlashFreq(0), flashTimeCounter(0),
timeUntilThrustIsAvailable(0.0), resetPortalRecollisionCountdown(-1), lastPortalCollidedWith(NULL) {
}

PaddleRemoteControlRocketProjectile::PaddleRemoteControlRocketProjectile(const PaddleRemoteControlRocketProjectile& copy) : 
RocketProjectile(copy), currAppliedAccelDir(copy.currAppliedAccelDir), currAppliedAccelMag(copy.currAppliedAccelMag),
currAppliedThrust(copy.currAppliedThrust), currFuelAmt(copy.currFuelAmt), currFlashColourAmt(copy.currFlashColourAmt), 
currFlashFreq(copy.currFlashFreq), flashTimeCounter(copy.flashTimeCounter), timeUntilThrustIsAvailable(copy.timeUntilThrustIsAvailable),
resetPortalRecollisionCountdown(copy.resetPortalRecollisionCountdown), lastPortalCollidedWith(copy.lastPortalCollidedWith) {
}

PaddleRemoteControlRocketProjectile::~PaddleRemoteControlRocketProjectile() {
}

void PaddleRemoteControlRocketProjectile::LevelPieceCollisionOccurred(LevelPiece* block) {
    // If we collide with a portal block then we'll need to set a timer to get rid of the last thing this
    // rocket collided with since the player could redirect the rocket back through the portal block and we want
    // to allow this...
    if (block->GetType() == LevelPiece::Portal) {
        this->resetPortalRecollisionCountdown = PORTAL_COLLISION_RESET_TIME;
        this->lastPortalCollidedWith = static_cast<PortalBlock*>(block)->GetSiblingPortal();
    }
    else {
        this->resetPortalRecollisionCountdown = -1;
        this->lastPortalCollidedWith = NULL;
    }
}

/// <summary>
/// Called by the GameModel when first setting up this projectile. Use this
/// function to manipulate the game state so that everything freezes and the player gets
/// control over this rocket.
/// </summary>
void PaddleRemoteControlRocketProjectile::Setup(GameModel& gameModel) {
    gameModel.ReleaseBulletTime();

    // The transform manager will take care of pausing the game state and doing the transforms...
    GameTransformMgr* transformMgr = gameModel.GetTransformInfo();
    assert(transformMgr != NULL);
    transformMgr->SetRemoteControlRocketCamera(true, this);
}

/// <summary>
/// Called by the GameModel when this projectile is just about to be deleted.
/// Use this function to clean up the game state from when it was frozen for the remote control take-over.
/// </summary>
void PaddleRemoteControlRocketProjectile::Teardown(GameModel& gameModel) {

    // The transform manager will take care of un-pausing the game state and doing the transforms...
    GameTransformMgr* transformMgr = gameModel.GetTransformInfo();
    assert(transformMgr != NULL);
    transformMgr->SetRemoteControlRocketCamera(false, NULL);

    // If the rocket went out of bounds then blow it up...
    if (gameModel.IsOutOfGameBoundsForProjectile(this->GetPosition())) {
        GameLevel* currLevel = gameModel.GetCurrentLevel();
        currLevel->RocketExplosionNoPieces(this);
    }
}

void PaddleRemoteControlRocketProjectile::Tick(double seconds, const GameModel& model) {
    
    if (this->IsLoadedInCannonBlock()) {
        // Remove all applied acceleration if the rocket is inside a cannon block...
        this->SetAppliedAcceleration(Vector2D(0,0));
        this->currAppliedThrust = 0.0f;
    }
    else {
        // Update the acceleration/velocity/position of the rocket based on the current steering
        if (!this->IsRocketStillTakingOff()) {
            this->SetVelocity(this->GetVelocity() + (seconds * this->GetAppliedAcceleration()) + 
                (seconds * this->currAppliedThrust * this->GetVelocityDirection()));
        }
        
        this->currAppliedAccelMag -= DECELLERATION_OF_APPLIED_ACCEL * seconds;
        if (this->currAppliedAccelMag <= 0.0f) {
            this->currAppliedAccelMag = 0.0f;
            this->currAppliedAccelDir = Vector2D(0,0);
        }

        this->currAppliedThrust -= THRUST_DECREASE_RATE * seconds;
        if (this->currAppliedThrust < 0.0f) {
            this->currAppliedThrust = 0.0f;
        }

        this->timeUntilThrustIsAvailable -= seconds;
        if (this->timeUntilThrustIsAvailable < 0.0) {
            this->timeUntilThrustIsAvailable = 0.0;
        }

        // Check to see if we should reset the last portal collision 
        if (this->resetPortalRecollisionCountdown != -1) {
            if (this->resetPortalRecollisionCountdown <= 0.0) {

                assert(this->lastPortalCollidedWith != NULL);
                if (this->lastThingCollidedWith == this->lastPortalCollidedWith ||
                    this->lastThingCollidedWith == this->lastPortalCollidedWith->GetSiblingPortal()) {
                    this->SetLastThingCollidedWith(NULL);
                }

                this->lastPortalCollidedWith = NULL;
                this->resetPortalRecollisionCountdown = -1;
            }
            else {
                this->resetPortalRecollisionCountdown -= seconds;
            }
        }
    }

    RocketProjectile::Tick(seconds, model);
}


/// <summary> Update the level based on changes to this projectile that can result in level changes. </summary>
/// <returns> true if this projectile has been destroyed/removed from the game, false otherwise.</returns>
bool PaddleRemoteControlRocketProjectile::ModifyLevelUpdate(double dT, GameModel& model) {
    // If the fuel is already gone then we do nothing: This rocket is already dead/exploded
    if (this->currFuelAmt <= 0.0f) {
        return true;
    }

    // Only do fuel calculations/changes if the rocket isn't inside a cannon block...
    if (!this->IsLoadedInCannonBlock()) {
        // Eat up some fuel and check to see whether we should explode yet or not...
        this->currFuelAmt -= dT * RATE_OF_FUEL_CONSUMPTION;
        if (this->currFuelAmt <= 0.0f) {
            // The rocket is out of fuel, explosion time!
            model.GetCurrentLevel()->RocketExplosionNoPieces(this);
            return true;
        }

        // Depending on how close we are to exploding, change the flashing amount...
        if (this->currFuelAmt <= FUEL_AMOUNT_TO_START_FLASHING) {
            
            this->currFlashFreq = NumberFuncs::LerpOverFloat(FUEL_AMOUNT_TO_START_FLASHING, 0.0f, 2.0f, 10.0f, this->currFuelAmt);
            float currFlashTime = 1.0f / this->currFlashFreq;

            this->flashTimeCounter   = std::min<float>(currFlashTime, this->flashTimeCounter + dT);
            this->currFlashColourAmt = std::min<float>(1.0f, NumberFuncs::LerpOverFloat(0.0f, 0.8f*currFlashTime, 0.0f, 1.0f, this->flashTimeCounter));

            if (this->flashTimeCounter >= currFlashTime) {
                this->flashTimeCounter = 0.0;
                // EVENT: Fuel is running out pulse event
                GameEventManager::Instance()->ActionRemoteControlRocketFuelWarning(*this);
            }
        }
    }

    return false;
}

void PaddleRemoteControlRocketProjectile::ControlRocketThrust(float magnitudePercent) {
    // Can't control a rocket if it's inside a cannon block or if it's very early in take-off
    if (this->IsLoadedInCannonBlock() || this->IsRocketStillTakingOff()) { 
        return;
    }
    float prevThrust = this->currAppliedThrust;
    float newThrust  = magnitudePercent * MAX_APPLIED_THRUST;

    if (this->timeUntilThrustIsAvailable <= 0.0) {
        this->currAppliedThrust = newThrust;
    }
    else {
        this->currAppliedThrust = std::max<float>(newThrust, prevThrust);
    }

    if (prevThrust == 0.0f && this->currAppliedThrust > 0.2f*MAX_APPLIED_THRUST) {
        if (this->timeUntilThrustIsAvailable <= 0.0) {
            // EVENT: Thrust was just applied to the rocket...
            GameEventManager::Instance()->ActionRemoteControlRocketThrustApplied(*this);
            this->timeUntilThrustIsAvailable = MIN_TIME_BETWEEN_THRUSTS;
        }
        else {
            this->currAppliedThrust = 0.0f;
        }
    }
    else if (this->currAppliedThrust == 0.0f && prevThrust > 0.0f) {
        // Put a tiny wait in between thrusts, this makes sure that the emitters
        // don't go haywire and spam particles when the player is pulsing the controls
        this->timeUntilThrustIsAvailable = 0.1;
    }
}