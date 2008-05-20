#include "GameModel.h"
#include "BallOnPaddleState.h"
#include "GameEventManager.h"
#include "GameConstants.h"

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
	this->currState = new BallOnPaddleState(this);
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

void GameModel::Tick(double seconds) {
	this->currState->Tick(seconds);
}