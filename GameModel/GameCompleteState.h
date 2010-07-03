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