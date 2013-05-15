/**
 * GameOverState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

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