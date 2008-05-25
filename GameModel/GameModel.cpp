#include "GameModel.h"
#include "GameEventManager.h"
#include "GameConstants.h"

#include "BallOnPaddleState.h"
#include "GameCompleteState.h"

#include "../Utils/Includes.h"

GameModel::GameModel() : currWorldNum(0), currState(NULL) {
	
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

void GameModel::BeginOrRestartGame() {
	// TODO: move stuff into states and don't keep it here!!
	this->SetCurrentState(new BallOnPaddleState(this));
	this->SetCurrentWorld(GameConstants::INITIAL_WORLD_NUM);
}

void GameModel::SetCurrentWorld(unsigned int worldNum) {

	this->currWorldNum = worldNum;
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

	if (currWorld->IsLastLevel()) {
		// The last level of the world... need to check for
		// end of the game state
		if (this->IsLastWorld()) {
			// It's the end of the game, no more levels, just switch states
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
	this->currState->Tick(seconds);
}