/**
 * WorldCompleteState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __WORLDCOMPLETESTATE_H__
#define __WORLDCOMPLETESTATE_H__

#include "GameState.h"

class WorldCompleteState : public GameState {

public:
	WorldCompleteState(GameModel* gm);
	~WorldCompleteState();

	GameState::GameStateType GetType() const {
		return GameState::WorldCompleteStateType;
	}

	void Tick(double seconds);
	void BallReleaseKeyPressed() { /* does nothing */ } 

};

#endif // __WORLDCOMPLETESTATE_H__