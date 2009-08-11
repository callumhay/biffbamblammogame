#include "BallOnPaddleState.h"
#include "BallInPlayState.h"
#include "GameModel.h"
#include "GameBall.h"
#include "GameEventManager.h"

BallOnPaddleState::BallOnPaddleState(GameModel* gm) : GameState(gm), firstTick(true) {
	assert(gm != NULL);
	
	PlayerPaddle* paddle = this->gameModel->GetPlayerPaddle();
	assert(paddle != NULL);
	GameBall* ball = this->GetGameBall();
	assert(ball != NULL);

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
	GameBall* ball				= this->GetGameBall();
	PlayerPaddle* paddle	= this->gameModel->GetPlayerPaddle();

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
	this->UpdateBallPosition();

	// If this is the first tick of the current state, then the ball has just
	// been spawned on the player paddle
	if (this->firstTick) {
		this->firstTick = false;

		// EVENT: Ball (Re)spawning
		GameEventManager::Instance()->ActionBallSpawn(*this->GetGameBall());
	}
}

/**
 * When the player presses a control to release the ball from the paddle.
 */
void BallOnPaddleState::BallReleaseKeyPressed() {
	
	// Make sure the ball's position is updated first
	this->UpdateBallPosition();

	// Fire the ball off the paddle
	this->gameModel->GetPlayerPaddle()->Shoot(this->gameModel);

	// Now change the game model's state machine to have the ball in play
	this->gameModel->SetCurrentState(new BallInPlayState(this->gameModel));
}