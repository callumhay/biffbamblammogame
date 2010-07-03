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