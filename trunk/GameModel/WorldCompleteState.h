#ifndef __WORLDCOMPLETESTATE_H__
#define __WORLDCOMPLETESTATE_H__

#include "GameState.h"

class WorldCompleteState : public GameState {

public:
	WorldCompleteState(GameModel* gm);
	~WorldCompleteState();

	void Tick(double seconds);
	void BallReleaseKeyPressed() { /* does nothing */ } 

};

#endif // __WORLDCOMPLETESTATE_H__