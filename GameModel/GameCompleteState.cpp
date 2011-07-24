/**
 * GameCompleteState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameCompleteState.h"

GameCompleteState::GameCompleteState(GameModel* gm) : GameState(gm) {
}

GameCompleteState::~GameCompleteState() {
}

void GameCompleteState::Tick(double seconds) {
	UNUSED_PARAMETER(seconds);
}