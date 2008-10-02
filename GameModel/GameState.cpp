#include "GameState.h"
#include "GameModel.h"

/**
 * Default action here is to just move the paddle around.
 */
void GameState::MovePaddleKeyPressed(float dist) {
	this->gameModel->GetPlayerPaddle()->Move(dist);
}