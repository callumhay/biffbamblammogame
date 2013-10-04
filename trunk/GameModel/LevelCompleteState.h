/**
 * LevelCompleteState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

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
	void ShootActionReleaseUse() {
        // does nothing
    } 

private:
    bool waitingForExternalExit;
    DISALLOW_COPY_AND_ASSIGN(LevelCompleteState);

};

#endif // __LEVELCOMPLETESTATE_H__