/**
 * BallOnPaddleState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "BallOnPaddleState.h"
#include "BallInPlayState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

const double BallOnPaddleState::START_RELEASE_TIMER_TIME_IN_SECS = 2.5;
const double BallOnPaddleState::TOTAL_RELEASE_TIMER_TIME_IN_SECS = START_RELEASE_TIMER_TIME_IN_SECS + 6.0;

BallOnPaddleState::BallOnPaddleState(GameModel* gm, double pauseTime) : GameState(gm), 
firstTick(true), releaseTimerStarted(false), releaseTimerCounter(0.0), waitTimerCounter(0.0),
timeToWaitBeforeReleaseIsAllowed(pauseTime), ballWasReleased(false) {

	assert(gm != NULL);

	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);

    // Make sure there's only one ball, then get that one ball
    this->gameModel->ClearBallsToOne();
	GameBall* ball = this->GetGameBall();
	assert(ball != NULL);


	GameLevel* currLevel = this->gameModel->GetCurrentLevel();
	assert(currLevel != NULL);

	// Make sure the paddle and ball are in a nice default state and position
	paddle->UpdateLevel(*currLevel);
	paddle->ResetPaddle();
	ball->ResetBallAttributes();

	this->UpdateBallPosition();	// This must be called before attaching the ball - makes sure the ball position is synced to the paddle!!
	paddle->AttachBall(ball);
}

BallOnPaddleState::~BallOnPaddleState() {
}

/**
 * Private helper function - since the model stores the game balls in a list we need to make sure
 * that there is ONLY one ball and retrieve only that ball.
 * Returns: The only game ball currently in existence within the game model.
 */
GameBall* BallOnPaddleState::GetGameBall() {
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	assert(gameBalls.size() == 1);
	return *gameBalls.begin();
}

/**
 * Private helper function that updates the ball position based on the movement
 * of the player paddle.
 */
void BallOnPaddleState::UpdateBallPosition() {
	GameBall* ball       = this->GetGameBall();
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();

	Point2D ballLoc = paddle->GetCenterPosition() + (ball->GetBounds().Radius() + paddle->GetHalfHeight() + 0.05f) * paddle->GetUpVector();
	ball->SetCenterPosition(ballLoc);
}

/**
 * Time tick in-game as dealt with when the ball is on the paddle only.
 */
void BallOnPaddleState::Tick(double seconds) {

    // Make sure the ball is attached if it hasn't been released yet
    if (!ballWasReleased) {
        PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	    assert(paddle != NULL);
	    GameBall* ball = this->GetGameBall();
        paddle->AttachBall(ball);
    }

	// Since some piece may have status effects which cause projectiles to spawn and move around (e.g., fire globs)
	// we still need to tick projectiles in this state... 
	this->gameModel->UpdateActiveProjectiles(seconds);
    this->gameModel->UpdateActiveBeams(seconds);

	// Make sure the paddle isn't moving into any blocks along the sides of the level
	this->DoUpdateToPaddleBoundriesAndCollisions(seconds, true);

	// Projectile Collisions:
	this->gameModel->DoProjectileCollisions(seconds);
	// Tick/update any level pieces that require it...
	this->gameModel->DoPieceStatusUpdates(seconds);

	// In this state the paddle can move around freely and the ball stays at
	// its center without moving.
	this->UpdateBallPosition();
	
	//GameBall* ball = this->GetGameBall();
	//ball->Tick(seconds);

	// If this is the first tick of the current state, then the ball has just
	// been spawned on the player paddle
	if (this->firstTick) {
		this->firstTick = false;
        this->releaseTimerCounter = 0.0;
        this->waitTimerCounter = 0.0;

		// EVENT: Ball (Re)spawning
		GameEventManager::Instance()->ActionBallSpawn(*this->GetGameBall());
	}
    else if (PlayerPaddle::GetIsPaddleReleaseTimerEnabled()) {
        // Logic for the release timer and wait
        this->releaseTimerCounter += seconds;

        // Check to see whether we should start the release timer yet, we only send an event for
        // right when it starts
        if (this->releaseTimerCounter > START_RELEASE_TIMER_TIME_IN_SECS && !releaseTimerStarted) {
            GameEventManager::Instance()->ActionReleaseTimerStarted();
            this->releaseTimerStarted = true;
        }
        // If the timer expires then the ball is automatically released
        else if (this->releaseTimerCounter >= TOTAL_RELEASE_TIMER_TIME_IN_SECS) {
            this->ShootActionReleaseUse();
        }
    }

    this->waitTimerCounter += seconds;

    // Update AI entities
    if ((this->gameModel->GetPauseState() & GameModel::PauseAI) == 0x00000000) {
        this->gameModel->GetCurrentLevel()->TickAIEntities(seconds, this->gameModel);
    }
}

/**
 * When the player presses a control to release the ball from the paddle.
 */
void BallOnPaddleState::ShootActionReleaseUse() {

    // If the button wasn't pressed hard enough or a crucial element of the game is paused then
    // we don't release the ball from the paddle
    if ((this->gameModel->GetPauseState() & (GameModel::PausePaddle | GameModel::PausePaddleControls)) != 0x0 ||
        (this->gameModel->GetPauseState() & GameModel::PauseBall) != 0x0) {
        return;
    } 

	// Make sure the ball's position is updated first
	this->UpdateBallPosition();

    // Only fire the ball if it's enabled
    if (PlayerPaddle::GetIsPaddleReleaseEnabled()) {

        // If we're paused before firing then we can't fire
        if (this->waitTimerCounter < this->timeToWaitBeforeReleaseIsAllowed) {
            return;
        }

	    // Fire the ball off the paddle
	    this->gameModel->GetPlayerPaddle()->DiscreteShoot(this->gameModel);
	    // Now change the game model's state machine to have the ball in play
        this->gameModel->SetNextState(GameState::BallInPlayStateType);

        this->ballWasReleased = true;

        // Update the paddle boundaries
        PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
        const GameLevel* currLevel = this->gameModel->GetCurrentLevel(); 
        paddle->UpdatePaddleBounds(currLevel->GetPaddleMinXBound(0), currLevel->GetPaddleMaxXBound(0));
    }
}

void BallOnPaddleState::ToggleAllowPaddleBallLaunching(bool allow) {
    // Enable/Disable the release of the ball from the paddle
    PlayerPaddle::SetEnablePaddleRelease(allow);
    // Enable/Disable the timer for releasing the ball
    PlayerPaddle::SetEnablePaddleReleaseTimer(allow);
}