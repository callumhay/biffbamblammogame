#include "BallOnPaddleState.h"
#include "BallInPlayState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

#include "../BlammoEngine/BlammoEngine.h"

BallOnPaddleState::BallOnPaddleState(GameModel* gm) : GameState(gm), firstTick(true) {
	this->UpdateBallPosition();
}

BallOnPaddleState::~BallOnPaddleState() {
}

void BallOnPaddleState::UpdateBallPosition() {
	GameBall* ball				=  this->gameModel->GetGameBall();
	PlayerPaddle* paddle	=  this->gameModel->GetPlayerPaddle();

	Vector2D disp = Vector2D(0.0f, ball->GetBounds().Radius() + paddle->GetHalfHeight() + 0.1f);
	Point2D ballLoc = paddle->GetCenterPosition() + disp;
	ball->SetCenterPosition(ballLoc);
}

/**
 * Time tick in-game as dealt with when the ball is on the paddle only.
 */
void BallOnPaddleState::Tick(double seconds) {
	// In this state the paddle can move around freely and the ball stays at
	// its center without moving.
	this->gameModel->GetPlayerPaddle()->Tick(seconds);
	this->UpdateBallPosition();
	
	// If this is the first tick of the current state, then the ball has just
	// been spawned on the player paddle
	if (this->firstTick) {
		this->firstTick = false;
		// EVENT: Ball (Re)spawning
		GameEventManager::Instance()->ActionBallSpawn(*this->gameModel->GetGameBall());
	}
}

/**
 * When the player presses a control to release the ball from the paddle.
 */
void BallOnPaddleState::BallReleaseKeyPressed() {

	Vector2D ballReleaseDir = GameBall::STD_INIT_VEL_DIR;

	// Get the paddle's avg. velocity
	Vector2D avgPaddleVel = this->gameModel->GetPlayerPaddle()->GetAvgVelocity();
	// Check to see if the paddle is moving, if not just use a random angle
	if (fabs(avgPaddleVel[0]) <= EPSILON) {
		// Add some randomness to the velocity by deviating a straight-up shot by some random angle
		float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::STILL_RAND_RELEASE_DEG;		
		ballReleaseDir = Rotate(randomAngleInDegs, ballReleaseDir);
	}
	else {
		// The paddle appears to be moving, modify the ball's release velocity
		// to reflect some of this movement
		float multiplier = PlayerPaddle::DEFAULT_SPEED / static_cast<float>(GameBall::NormalSpeed);
		Vector2D newBallDir = Vector2D::Normalize(ballReleaseDir + multiplier * avgPaddleVel);
		
		// and, of course, add some randomness...
		float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::MOVING_RAND_RELEASE_DEG;		
		ballReleaseDir = Rotate(randomAngleInDegs, newBallDir);
	}

	ballReleaseDir.Normalize();
	this->gameModel->GetGameBall()->SetVelocity(GameBall::NormalSpeed, ballReleaseDir);

	// EVENT: Ball Shot
	GameEventManager::Instance()->ActionBallShot(*this->gameModel->GetGameBall());

	// Now change the game model's state machine to have the ball in play
	this->gameModel->SetCurrentState(new BallInPlayState(this->gameModel));
}