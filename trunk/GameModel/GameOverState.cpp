#include "GameOverState.h"
#include "GameModel.h"

GameOverState::GameOverState(GameModel* gm) : GameState(gm) {
}

GameOverState::~GameOverState(){
	// If we are exiting being in play then clear all projectiles, items and timers
	this->gameModel->ClearProjectiles();
	this->gameModel->ClearBeams();
	this->gameModel->ClearLiveItems();
	this->gameModel->ClearActiveTimers();
	this->gameModel->ClearStatusUpdatePieces();
}