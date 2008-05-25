#ifndef __GAMECOMPLETESTATE_H__
#define __GAMECOMPLETESTATE_H__

#include "GameState.h"

class GameCompleteState : public GameState {



public:
	GameCompleteState(GameModel* gm);
	virtual ~GameCompleteState();

	virtual void Tick(double seconds);
	virtual void BallReleaseKeyPressed() {
		// Do nothing, the game is over!
	}

};

#endif