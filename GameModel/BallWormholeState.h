/**
 * BallWormholeState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLWORMHOLESTATE_H__
#define __BALLWORMHOLESTATE_H__

#include "GameState.h"

#include <list>

class GameBall;

class BallWormholeState : public GameState {
public:
	BallWormholeState(GameModel* gm);
	~BallWormholeState();

	GameState::GameStateType GetType() const {
		return GameState::BallWormholeStateType;
	}

	void Tick(double seconds);
	void MovePaddleKeyPressed(const PlayerPaddle::PaddleMovement& paddleMovement);
	void BallReleaseKeyPressed() {}

private:
	GameBall* wormholeBall;
	std::list<GameBall*> originalBalls;

};

#endif // __BALLWORMHOLESTATE_H__