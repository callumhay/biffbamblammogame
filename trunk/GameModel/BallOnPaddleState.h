/**
 * BallOnPaddleState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLONPADDLESTATE_H__
#define __BALLONPADDLESTATE_H__

#include "GameState.h"

class GameBall;

class BallOnPaddleState : public GameState {

private:
	bool firstTick;
	void UpdateBallPosition();

	GameBall* GetGameBall();

public:
	BallOnPaddleState(GameModel* gm);
	~BallOnPaddleState();

	GameState::GameStateType GetType() const {
		return GameState::BallOnPaddleStateType;
	}

	void Tick(double seconds);
	void BallReleaseKeyPressed();

};
#endif