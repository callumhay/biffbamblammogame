#include "GameModel.h"
#include "BallOnPaddleState.h"

#include "../Utils/Includes.h"

GameModel::GameModel() : currWorldNum(0), currState(NULL) {
	this->currState = new BallOnPaddleState(this);
	this->worlds.push_back(new GameWorld("resources/levels/deco/deco.wld"));
	this->SetCurrentWorld(this->currWorldNum);
}

GameModel::~GameModel() {
	for (size_t i = 0; i < this->worlds.size(); i++) {
		delete this->worlds[i];
	}
	delete this->currState;
}

void GameModel::SetCurrentWorld(unsigned int worldNum) {

	GameWorld* world = this->worlds[worldNum];
	assert(world != NULL);
	if (!world->Load()) {
		debug_output("ERROR: Could not load world 0.");
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