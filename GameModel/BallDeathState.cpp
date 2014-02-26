/**
 * BallDeathState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BallDeathState.h"
#include "GameOverState.h"
#include "BallOnPaddleState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"
#include "GameTransformMgr.h"

const double BallDeathState::SPIRAL_ANIMATION_TIME_TOTAL    = 1.25;	// Total time of the spiral animation in seconds
const double BallDeathState::EXPLOSION_ANIMATION_TIME_TOTAL	= 0.75;	// Total time of the explosion animation in seconds

BallDeathState::BallDeathState(GameBall* lastBallToBeAlive, GameModel* gm) : 
GameState(gm), spiralRadius(0.0f), lastBallToBeAlive(lastBallToBeAlive), timeElapsed(0.0),
currAnimationState(BallDeathState::SpiralingToDeath) {
	assert(lastBallToBeAlive != NULL);

	// EVENT: Last ball is about to die
	GameEventManager::Instance()->ActionLastBallAboutToDie(*this->lastBallToBeAlive);

	this->initialBallPosition    = this->lastBallToBeAlive->GetBounds().Center();
	this->initialBallVelocityDir = this->lastBallToBeAlive->GetDirection();
	this->initialBallVelocityMag = this->lastBallToBeAlive->GetSpeed();

	// Disable any effects currently on the ball...
	this->lastBallToBeAlive->RemoveAllBallTypes();

	// Completely pause the paddle since the ball death is a non-controllable happening
	this->gameModel->SetPause(GameModel::PausePaddle);
	// Move the camera to follow the ball as it dies
	this->gameModel->GetTransformInfo()->SetBallDeathCamera(true);
	// Make sure the ball is visible
	this->lastBallToBeAlive->SetAlpha(1.0f);

	// Setup any animations for the ball
	this->spiralAnimation = AnimationLerp<float>(&this->spiralRadius);
	this->spiralAnimation.SetRepeat(false);
	this->spiralAnimation.SetLerp(0.0, BallDeathState::SPIRAL_ANIMATION_TIME_TOTAL, 0.5, 3.0);

    // Remove any mine that might be attached to something that is being removed
    this->gameModel->RemoveBallDeathMineProjectiles();

	// Clear all the falling items out of the level - makes sure all their sounds/effects
	// are eliminated so the player only sees and hears his/her death animation
	this->gameModel->ClearLiveItems();
}

BallDeathState::~BallDeathState() {
	// Place the camera back in its default position
	this->gameModel->GetTransformInfo()->SetBallDeathCamera(false);	
	
	// If we are exiting being in play then clear all items and timers
	this->gameModel->ClearBeams();
	this->gameModel->ClearActiveTimers();
	
    // Remove any rockets attached to the paddle
    this->gameModel->GetPlayerPaddle()->RemovePaddleType(PlayerPaddle::RocketPaddle);
    this->gameModel->GetPlayerPaddle()->RemovePaddleType(PlayerPaddle::RemoteControlRocketPaddle);

    // Only clear projectiles if it's game over
    if (this->gameModel->IsGameOver()) {
        this->gameModel->ClearProjectiles();
    }

	// Allow the player to control the paddle again
	this->gameModel->UnsetPause(GameModel::PausePaddle);
}

void BallDeathState::ShootActionReleaseUse() {
    // If this is being pressed then the user wants to skip the death animation...

    // We don't let players skip the final death animation
    if (this->gameModel->GetLivesLeft() <= 1) {
        return;
    }

    switch (this->currAnimationState) {

        case BallDeathState::SpiralingToDeath: {
            // We need to clean some stuff up first and signal some events/functions to make sure
            // all the 'housekeeping' is taken care of
            
            // EVENT: Ball just exploded, player skipped it...
            GameEventManager::Instance()->ActionLastBallExploded(*this->lastBallToBeAlive, true);

            bool stateChanged = false;
            this->gameModel->BallDied(this->lastBallToBeAlive, stateChanged);
            assert(stateChanged);

            // The state has officially changed, get out of here immediately!
            return;
        }

        case BallDeathState::Exploding:
            // Don't wait for the explosion, go immediately to the dead state
            this->timeElapsed = BallDeathState::EXPLOSION_ANIMATION_TIME_TOTAL;
            this->currAnimationState = BallDeathState::Dead;
            break;

        case BallDeathState::Dead:
            // Ignore the button press, we're going back to gameplay anyway
            break;

        default:
            assert(false);
            break;
    }
}

/**
 * Follow the ball down into the abyss and then have it spiral and explodinate.
 */
void BallDeathState::Tick(double seconds) {
	// Animate the ball based on its current animation state... starts to spiral and shake then explode...
	switch (this->currAnimationState) {

		case BallDeathState::SpiralingToDeath:
			this->ExecuteSpiralingToDeathState(seconds);
			return;

		case BallDeathState::Exploding:
			// We need to just wait for the explosion...
			this->timeElapsed += seconds;
			if (this->timeElapsed >= BallDeathState::EXPLOSION_ANIMATION_TIME_TOTAL) {
				this->currAnimationState = BallDeathState::Dead;
			}
			return;

		case BallDeathState::Dead: {
				bool stateChanged = false;
				this->gameModel->BallDied(this->lastBallToBeAlive, stateChanged);
				assert(stateChanged);
                // Place the ball off camera for the time being
                this->lastBallToBeAlive->SetCenterPosition(Point2D(99999,99999));
			}
			return;
		
		default:
			assert(false);
			break;
	}
}

/**
 * Private helper function for executing the spiral to death animation state.
 */
void BallDeathState::ExecuteSpiralingToDeathState(double dT) {
	// Grab the distance along the circle of the spiral
	float circleDistTravelled = this->initialBallVelocityMag * this->timeElapsed;

	// Figure out which way to make it spiral
	float spiralXMultiplier	= -NumberFuncs::SignOf(this->initialBallVelocityDir[0]);
	float modifiedRadius    = spiralXMultiplier * this->spiralRadius;

	// To make the ball spiral we use a parametric equation:
	Vector2D ballTranslation(modifiedRadius * cos(circleDistTravelled) - modifiedRadius, 
        (-this->spiralRadius * sin(circleDistTravelled)));

	Point2D newBallPos = this->initialBallPosition + ballTranslation + 
        (0.85f * this->initialBallVelocityMag * this->timeElapsed * this->initialBallVelocityDir);
	this->lastBallToBeAlive->SetCenterPosition(newBallPos);
	
	// Check to see if the spiral animation is completed... if it has then we move onto the next state
	bool isFinished = this->spiralAnimation.Tick(dT);
	if (isFinished) {
		this->timeElapsed = 0.0;
		this->currAnimationState = BallDeathState::Exploding;
		
		// EVENT: Ball just exploded...
		GameEventManager::Instance()->ActionLastBallExploded(*this->lastBallToBeAlive, false);

		// Make the ball disappear as it explodes and also (sort of) stop moving
		this->lastBallToBeAlive->SetAlpha(0.0f);
        this->lastBallToBeAlive->SetSpeed(GameBall::GetSlowestSpeed());
	}
	else {
		timeElapsed += dT;
	}
}