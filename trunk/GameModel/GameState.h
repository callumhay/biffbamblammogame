/**
 * GameState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class GameBall;

class GameState {
public:
	virtual ~GameState() {}

	enum GameStateType { NULLStateType, BallDeathStateType, BallInPlayStateType, BallOnPaddleStateType, 
                         BallWormholeStateType, GameCompleteStateType, GameOverStateType, 
                         LevelStartStateType, LevelCompleteStateType, WorldCompleteStateType };

    static GameState* Build(const GameState::GameStateType& stateType, GameModel* gameModel);

    static bool IsGameInPlayState(const GameModel& gameModel);

	virtual GameState::GameStateType GetType() const = 0;
	virtual void Tick(double seconds) = 0;
    virtual void ShootActionReleaseUse() = 0;
    virtual void ShootActionContinuousUse(float magnitudePercent) { UNUSED_PARAMETER(magnitudePercent); }
	virtual void MoveKeyPressedForPaddle(int dir, float magnitudePercent);
    virtual void MoveKeyPressedForOther(int dir, float magnitudePercent) { UNUSED_PARAMETER(dir); UNUSED_PARAMETER(magnitudePercent); }
    virtual void BallBoostDirectionPressed(int x, int y);
    virtual void BallBoostDirectionReleased();
    virtual void BallBoostReleasedForBall(const GameBall& ball);

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

inline void GameState::BallBoostReleasedForBall(const GameBall&) {
    // Do nothing by default
}

#endif