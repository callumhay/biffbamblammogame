#ifndef __LEVELCOMPLETESTATE_H__
#define __LEVELCOMPLETESTATE_H__

#include "GameState.h"

class LevelCompleteState : public GameState {

public:
	LevelCompleteState(GameModel* gm);
	~LevelCompleteState();

	GameState::GameStateType GetType() const {
		return GameState::LevelCompleteStateType;
	}

	void Tick(double seconds);
	void BallReleaseKeyPressed() { /* does nothing */ } 

};

#endif // __LEVELCOMPLETESTATE_H__