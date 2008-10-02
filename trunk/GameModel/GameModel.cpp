#include "GameModel.h"

#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"

#include "../Utils/Includes.h"

GameModel::GameModel() : currWorldNum(0), currState(NULL), currPlayerScore(0), currLivesLeft(0) {
	
	// Initialize the worlds for the game
	for (size_t i = 0; i < GameConstants::WORLD_PATHS.size(); i++) {
		this->worlds.push_back(new GameWorld(GameConstants::WORLD_PATHS[i]));
	}

	// Initialize paddle and ball
	this->playerPaddle = new PlayerPaddle();
	this->ball = new GameBall();
}

GameModel::~GameModel() {
	// Delete all loaded worlds
	for (size_t i = 0; i < this->worlds.size(); i++) {
		delete this->worlds[i];
		this->worlds[i] = NULL;
	}
	
	// Delete the state
	delete this->currState;
	this->currState = NULL;

	// Delete ball and paddle
	delete this->ball;
	this->ball = NULL;
	delete this->playerPaddle;
	this->playerPaddle = NULL;
}

/**
 * Called when we want to begin/restart the model from the
 * beginning of the game, this will reinitialize the model to
 * the first world, level, etc.
 */
void GameModel::BeginOrRestartGame() {
	this->SetCurrentState(new BallOnPaddleState(this));
	this->SetCurrentWorld(GameConstants::INITIAL_WORLD_NUM);

	// Reset the score, lives, etc.
	this->currPlayerScore					= GameConstants::INIT_SCORE;
	this->currLivesLeft						= GameConstants::INIT_LIVES;
	this->numConsecutiveBlocksHit = GameConstants::DEFAULT_BLOCKS_HIT;	// Don't use set here, we don't want an event
}

void GameModel::SetCurrentWorld(unsigned int worldNum) {
	assert(worldNum < this->worlds.size());
	
	// Clean up the previous world
	GameWorld* prevWorld = this->GetCurrentWorld();
	prevWorld->Unload();

	// Get the world we want to set as current
	GameWorld* world = this->worlds[worldNum];
	assert(world != NULL);
	
	// Make sure the world loaded properly.
	if (!world->Load()) {
		debug_output("ERROR: Could not load world " << worldNum);
		assert(false);
		return;
	}

	this->currWorldNum = worldNum;

	GameLevel* currLevel = world->GetCurrentLevel();
	assert(currLevel != NULL);

	this->playerPaddle->SetMinMaxLevelBound(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());
}

/**
 * Determines the next level to increment to and then loads the appropriate
 * level, world or end of game depending on what comes next.
 */
void GameModel::IncrementLevel() {
	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameConstants::DEFAULT_BLOCKS_HIT);

	GameWorld* currWorld = this->GetCurrentWorld();
	GameLevel* currLevel = currWorld->GetCurrentLevel();

	// EVENT: Level is complete
	GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel);

	if (currWorld->IsLastLevel()) {
		// EVENT: World is complete
		GameEventManager::Instance()->ActionWorldCompleted(*currWorld);

		// The last level of the world... need to check for end of the game state
		if (this->IsLastWorld()) {
			// EVENT: Game is complete
			GameEventManager::Instance()->ActionGameCompleted();

			// No more levels, just switch states
			this->SetCurrentState(new GameCompleteState(this));
		}
		else {
			// Increment to the next world
			this->SetCurrentWorld(this->currWorldNum + 1); 
			// Place the ball back on the paddle
			this->SetCurrentState(new BallOnPaddleState(this));
		}
	}
	else {
		// Increment to the next level, the world hasn't changed
		currWorld->IncrementLevel();
		// Place the ball back on the paddle
		this->SetCurrentState(new BallOnPaddleState(this));
	}
}

void GameModel::Tick(double seconds) {
	
	if (currState != NULL) {
		this->currState->Tick(seconds);
	}
	
}

/**
 * Called when a collision occurs between the ball and a level piece.
 * Deals with all the important effects a collision could have on the game model.
 * Precondition: p != NULL
 */
void GameModel::BallPieceCollisionOccurred(LevelPiece* p) {
	assert(p != NULL);

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionBallBlockCollision(*this->ball, *p);
	
	// Set the score appropriately
	int pointValue = p->GetPointValueForCollision();
	assert(pointValue >= 0);
	this->IncrementScore(pointValue);

	// Tell the level about the collision
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	currLevel->BallCollisionOccurred(p->GetHeightIndex(), p->GetWidthIndex());

	// If the piece was destroyed then increase the multiplier
	if (p->GetType() == LevelPiece::Empty) {
		this->SetNumConsecutiveBlocksHit(this->numConsecutiveBlocksHit+1);
	}

	// Check to see if the level is done
	if (currLevel->IsLevelComplete()) {

		// The level was completed, increment the level - note this function
		// is robust and will increment level, world and determine if we have finished
		// the game all in one
		this->IncrementLevel();
	}
}

void GameModel::BallPaddleCollisionOccurred() {
	// EVENT: Ball-paddle collision
	GameEventManager::Instance()->ActionBallPaddleCollision(*this->ball, *this->playerPaddle);

	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameConstants::DEFAULT_BLOCKS_HIT);

	// Modify the ball velocity using the current paddle velocity
	Vector2D paddleVel = this->playerPaddle->GetAvgVelocity();
	
	if (fabs(paddleVel[0]) > EPSILON) {
		// The paddle has a considerable velocity, we should augment the ball's
		// tragectory based on this (transfer of momentum)
		int angleDecSgn = -NumberFuncs::SignOf(paddleVel[0]);
		float fractionOfSpeed = fabs(paddleVel[0]) / PlayerPaddle::DEFAULT_SPEED;
		float angleChange = angleDecSgn * fractionOfSpeed * PlayerPaddle::RAND_DEG_ANG;

		// Set the ball to be exactly on the paddle
		this->ball->SetCenterPosition(Point2D(this->ball->GetBounds().Center()[0], 
			this->playerPaddle->GetCenterPosition()[1] + this->playerPaddle->GetHalfHeight() + this->ball->GetBounds().Radius()));

		Vector2D ballVel    = this->ball->GetVelocity();
		Vector2D ballVelHat = Vector2D::Normalize(ballVel);
		GameBall::BallSpeed ballSpd = ball->GetSpeed(); 

		// Rotate the ball's velocity vector to reflect the momentum of the paddle
		ball->SetVelocity(ballSpd, Rotate(angleChange, ballVelHat));
		
		// Make sure the ball goes upwards (it can't reflect downwards off the paddle or the game would suck)
		if (acosf(Vector2D::Dot(ballVelHat, Vector2D(0, 1))) > ((M_PI / 2.0f) - GameBall::MIN_BALL_ANGLE_IN_RADS)) {
			// Inline: The ball either at a very sharp angle w.r.t. the paddle or it is aimed downwards
			// even though the paddle has deflected it, adjust the angle to be suitable for the game
			if (ballVel[0] < 0) {
				this->ball->SetVelocity(ballSpd, Rotate(-GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(-1, 0)));
			}
			else {
				this->ball->SetVelocity(ballSpd, Rotate(GameBall::MIN_BALL_ANGLE_IN_DEGS, Vector2D(1, 0)));
			}
		}
	}
}

/**
 * Called when the player/ball dies (falls into the bottomless pit of doom). 
 * This will deal with all the effects a death has on the game model.
 */
void GameModel::PlayerDied() {
	// Reset the multiplier
	this->SetNumConsecutiveBlocksHit(GameConstants::DEFAULT_BLOCKS_HIT);
	
	// Decrement the number of lives left
	this->currLivesLeft--;

	// EVENT: Ball/Player death
	GameEventManager::Instance()->ActionBallDeath(*this->ball, this->currLivesLeft);

	// Set the appropriate state based on the number of lives left...
	if (this->IsGameOver()) {
		// Game Over
		this->SetCurrentState(new GameOverState(this));
	}
	else {
		this->SetCurrentState(new BallOnPaddleState(this));
	}
}