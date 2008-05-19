#include "BallOnPaddleState.h"
#include "BallInPlayState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

#include "../Utils/Point.h"
#include "../Utils/Algebra.h"

BallOnPaddleState::BallOnPaddleState(GameModel* gm) : GameState(gm), firstTick(true) {
}

BallOnPaddleState::~BallOnPaddleState() {
}

void BallOnPaddleState::UpdateBallPosition() {
	Vector2D disp = Vector2D(0.0f, this->gameModel->ball.GetBounds().Radius() + this->gameModel->playerPaddle.GetHalfHeight() + 0.1f);
	Point2D ballLoc = this->gameModel->playerPaddle.GetCenterPosition() + disp;
	this->gameModel->ball.SetCenterPosition(ballLoc);
}

/**
 * Time tick in-game as dealt with when the ball is on the paddle only.
 */
void BallOnPaddleState::Tick(double seconds) {
	// In this state the paddle can move around freely and the ball stays at
	// its center without moving.
	this->gameModel->playerPaddle.Tick(seconds);
	this->UpdateBallPosition();
	
	// If this is the first tick of the current state, then the ball has just
	// been spawned on the player paddle
	if (this->firstTick) {
		this->firstTick = false;
		// EVENT: Ball (Re)spawning
		GameEventManager::Instance()->ActionBallSpawn(this->gameModel->ball);
	}
}

/**
 * When the player presses a control to release the ball from the paddle.
 */
void BallOnPaddleState::BallReleaseKeyPressed() {
	
	// Add some randomness to the velocity by deviating a straight-up shot by some random angle
	Randomizer::InitializeRandomizer();
	float randomAngleInDegs = static_cast<float>(Randomizer::RandomNumNegOneToOne()) * GameBall::RAND_DEG_ANG;

	// Set the ball's initial velocity to be a bit in the direction of the 
	// paddle velocity with its initial magnitude
	int paddleDir = NumberFuncs::SignOf(this->gameModel->playerPaddle.GetAvgVelocity()[0]);
	if (paddleDir < 0) {
		randomAngleInDegs += PlayerPaddle::RAND_DEG_ANG;
	}
	else if (paddleDir > 0) {
		randomAngleInDegs -= PlayerPaddle::RAND_DEG_ANG;
	}

	Vector2D paddleVel = 0.5f*this->gameModel->playerPaddle.GetAvgVelocity();
	Vector2D slightlyRandomBallVel = Rotate(randomAngleInDegs, GameBall::STD_INIT_VEL);
	this->gameModel->ball.SetVelocity(slightlyRandomBallVel + paddleVel);
	
	// EVENT: Ball Shot
	GameEventManager::Instance()->ActionBallShot(this->gameModel->ball);

	// Now change the game model's state machine to have the ball in play
	this->gameModel->SetCurrentState(new BallInPlayState(this->gameModel));
}