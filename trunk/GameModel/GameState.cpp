/**
 * GameState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameState.h"
#include "GameModel.h"

/**
 * Default action here is to just move the paddle around.
 */
void GameState::MovePaddleKeyPressed(float dist) {
	this->gameModel->GetPlayerPaddle()->Move(dist);
}