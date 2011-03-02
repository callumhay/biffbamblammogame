/**
 * LevelCompleteState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
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
	void BallReleaseKeyPressed() { /* does nothing */ } 

};

#endif // __LEVELCOMPLETESTATE_H__