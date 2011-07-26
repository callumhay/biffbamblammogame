/**
 * GameCompleteState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMECOMPLETESTATE_H__
#define __GAMECOMPLETESTATE_H__

#include "GameState.h"

class GameCompleteState : public GameState {
public:
	GameCompleteState(GameModel* gm);
	~GameCompleteState();

	GameState::GameStateType GetType() const {
		return GameState::GameCompleteStateType;
	}

	void Tick(double seconds);
	void BallReleaseKeyPressed() {
		// Do nothing, the game is over!
	}
};

#endif