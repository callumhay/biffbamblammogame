/**
 * GameState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
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
public:
	virtual ~GameState() {}

	enum GameStateType { NULLStateType, BallDeathStateType, BallInPlayStateType, BallOnPaddleStateType, 
                         BallWormholeStateType, GameCompleteStateType, GameOverStateType, 
                         LevelStartStateType, LevelCompleteStateType, WorldCompleteStateType };

    static GameState* Build(const GameState::GameStateType& stateType, GameModel* gameModel);

	virtual GameState::GameStateType GetType() const = 0;
	virtual void Tick(double seconds) = 0;
	virtual void BallReleaseKeyPressed() = 0;
	virtual void MovePaddleKeyPressed(const PlayerPaddle::PaddleMovement& paddleMovement, float magnitudePercent);
    virtual void BallBoostDirectionPressed(int x, int y);
    virtual void BallBoostDirectionReleased();

	bool DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision);

protected:
    GameState(GameModel* gm) : gameModel(gm) {}

	GameModel* gameModel;

private:
    DISALLOW_COPY_AND_ASSIGN(GameState);
};

inline void GameState::BallBoostDirectionPressed(int x, int y) {
    UNUSED_PARAMETER(x);
    UNUSED_PARAMETER(y);
    // Do nothing by default
}

inline void GameState::BallBoostDirectionReleased() {
    // Do nothing by default
}

#endif