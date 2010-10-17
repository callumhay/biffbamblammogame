#ifndef __GAMEOVERSTATE_H__
#define __GAMEOVERSTATE_H__

#include "GameState.h"

class GameOverState : public GameState {

public:
	GameOverState(GameModel* gm);
	~GameOverState();

	GameState::GameStateType GetType() const {
		return GameState::GameOverStateType;
	}

	void Tick(double seconds) {
		UNUSED_PARAMETER(seconds);
		// Do nothing, player is dead.
	}
	void BallReleaseKeyPressed() {
		// Do nothing, ball is dead as is player.
	}

};
#endif