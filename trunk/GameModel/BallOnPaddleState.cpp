/**
 * BallOnPaddleState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BallOnPaddleState.h"
#include "BallInPlayState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

const double BallOnPaddleState::START_RELEASE_TIMER_TIME_IN_SECS = 2.5;
const double BallOnPaddleState::TOTAL_RELEASE_TIMER_TIME_IN_SECS = START_RELEASE_TIMER_TIME_IN_SECS + 6.0;

BallOnPaddleState::BallOnPaddleState(GameModel* gm) : GameState(gm), 
firstTick(true), releaseTimerStarted(false), releaseTimerCounter(0.0) {
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
	paddle->SetNewMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());
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
 * Returns: The only game ball currently in existance within the game model.
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

	Vector2D disp = Vector2D(0.0f, ball->GetBounds().Radius() + paddle->GetHalfHeight() + 0.1f);
	Point2D ballLoc = paddle->GetCenterPosition() + disp;
	ball->SetCenterPosition(ballLoc);
}

/**
 * Time tick in-game as dealt with when the ball is on the paddle only.
 */
void BallOnPaddleState::Tick(double seconds) {
	// Since some piece may have status effects which cause projectiles to spawn and move around (e.g., fire globs)
	// we still need to tick projectiles in this state... 
	this->gameModel->UpdateActiveProjectiles(seconds);

	// Make sure the paddle isn't moving into any blocks along the sides of the level
	this->DoUpdateToPaddleBoundriesAndCollisions(seconds, true);

	// Projectile Collisions:
	this->gameModel->DoProjectileCollisions();
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

		// EVENT: Ball (Re)spawning
		GameEventManager::Instance()->ActionBallSpawn(*this->GetGameBall());
	}
    else if (PlayerPaddle::GetIsPaddleReleaseTimerEnabled()) {
        // Logic for the release timer
        this->releaseTimerCounter += seconds;

        // Check to see whether we should start the release timer yet, we only send an event for
        // right when it starts
        if (this->releaseTimerCounter > START_RELEASE_TIMER_TIME_IN_SECS && !releaseTimerStarted) {
            GameEventManager::Instance()->ActionReleaseTimerStarted();
            this->releaseTimerStarted = true;
        }
        // If the timer expires then the ball is automatically released
        else if (this->releaseTimerCounter >= TOTAL_RELEASE_TIMER_TIME_IN_SECS) {
            this->BallReleaseKeyPressed();
        }
    }
}

/**
 * When the player presses a control to release the ball from the paddle.
 */
void BallOnPaddleState::BallReleaseKeyPressed() {
	
	// Make sure the ball's position is updated first
	this->UpdateBallPosition();

    // Only fire the ball if it's enabled
    if (PlayerPaddle::GetIsPaddleReleaseEnabled()) {
	    // Fire the ball off the paddle
	    this->gameModel->GetPlayerPaddle()->Shoot(this->gameModel);
	    // Now change the game model's state machine to have the ball in play
        this->gameModel->SetNextState(GameState::BallInPlayStateType);
    }
}