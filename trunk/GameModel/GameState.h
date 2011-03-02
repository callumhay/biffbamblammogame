/**
 * GameState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "PlayerPaddle.h"

class GameModel;

class GameState {
protected:
	GameModel* gameModel;

public:
	GameState(GameModel* gm) : gameModel(gm) {}
	virtual ~GameState() {}

	enum GameStateType { NULLStateType, BallDeathStateType, BallInPlayStateType, BallOnPaddleStateType, 
											 BallWormholeStateType, GameCompleteStateType, GameOverStateType, 
											 LevelCompleteStateType, WorldCompleteStateType };

	virtual GameState::GameStateType GetType() const = 0;

	virtual void Tick(double seconds) = 0;
	virtual void BallReleaseKeyPressed() = 0;
	virtual void MovePaddleKeyPressed(const PlayerPaddle::PaddleMovement& paddleMovement);

    //virtual void BallBoostDirectionPressed(int x, int y);
    //virtual void BallBoostDirectionReleased();

	bool DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision);

};

#endif