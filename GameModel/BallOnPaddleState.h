#ifndef __BALLONPADDLESTATE_H__
#define __BALLONPADDLESTATE_H__

#include "GameState.h"

class BallOnPaddleState : public GameState {

private:
	bool firstTick;
	void UpdateBallPosition();

public:
	BallOnPaddleState(GameModel* gm);
	virtual ~BallOnPaddleState();

	virtual void Tick(double seconds);
	virtual void BallReleaseKeyPressed();

};
#endif