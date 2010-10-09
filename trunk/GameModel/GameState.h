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
};

#endif