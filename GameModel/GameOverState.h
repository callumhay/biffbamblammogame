/**
 * GameOverState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

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