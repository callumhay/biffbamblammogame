#ifndef __GAMEOVERSTATE_H__
#define __GAMEOVERSTATE_H__

#include "GameState.h"

class GameOverState : public GameState {

public:
	GameOverState(GameModel* gm);
	~GameOverState();

	virtual void Tick(double seconds) {
		// Do nothing, player is dead.
	}
	virtual void BallReleaseKeyPressed() {
		// Do nothing, ball is dead as is player.
	}

};
#endif