#include "GameModel.h"
#include "GameConstants.h"

#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "GameOverState.h"

#include "../Utils/Includes.h"

GameModel::GameModel() : currWorldNum(0), currState(NULL), currPlayerScore(0), currLivesLeft(0) {
	
	// Initialize the worlds for the game
	for (size_t i = 0; i < GameConstants::WORLD_PATHS.size(); i++) {
		this->worlds.push_back(new GameWorld(GameConstants::WORLD_PATHS[i]));
	}
}

GameModel::~GameModel() {
	for (size_t i = 0; i < this->worlds.size(); i++) {
		delete this->worlds[i];
	}
	delete this->currState;
}

/**
 * Called when we want to begin/restart the model from the
 * beginning of the game, this will reinitialize the model to
 * the first world, level, etc.
 */
void GameModel::BeginOrRestartGame() {
	this->SetCurrentState(new BallOnPaddleState(this));
	this->SetCurrentWorld(GameConstants::INITIAL_WORLD_NUM);

	// Reset the score, lives
	this->currPlayerScore = GameConstants::INIT_SCORE;
	this->currLivesLeft		= GameConstants::INIT_LIVES;
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

	this->playerPaddle = PlayerPaddle(currLevel->GetPaddleMinBound(), currLevel->GetPaddleMaxBound());
}

/**
 * Determines the next level to increment to and then loads the appropriate
 * level, world or end of game depending on what comes next.
 */
void GameModel::IncrementLevel() {
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
void GameModel::BallCollisionOccurred(LevelPiece* p) {
	assert(p != NULL);

	// EVENT: Ball-Block Collision
	GameEventManager::Instance()->ActionBallBlockCollision(ball, *p);
	
	// Set the score appropriately
	this->IncrementScore(p->GetPointValueForCollision());

	// Tell the level about the collision
	GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
	currLevel->BallCollisionOccurred(p->GetHeightIndex(), p->GetWidthIndex());

	// Check to see if the level is done
	if (currLevel->IsLevelComplete()) {

		// The level was completed, increment the level - note this function
		// is robust and will increment level, world and determine if we have finished
		// the game all in one
		this->IncrementLevel();
	}
}

/**
 * Called when the player/ball dies (falls into the bottomless pit of doom). 
 * This will deal with all the effects a death has on the game model.
 */
void GameModel::PlayerDied() {
	this->currLivesLeft--;
	// EVENT: Ball/Player death
	GameEventManager::Instance()->ActionBallDeath(ball, this->currLivesLeft);

	// Set the appropriate state based on the number of lives left...
	if (this->IsGameOver()) {
		// Game Over
		this->SetCurrentState(new GameOverState(this));
	}
	else {
		this->SetCurrentState(new BallOnPaddleState(this));
	}
}