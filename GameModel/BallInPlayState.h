#ifndef __BALL_IN_PLAY_STATE_H__
#define __BALL_IN_PLAY_STATE_H__

#include <vector>

#include "GameState.h"

class GameBall;
class LineSeg2D;
class Vector2D;
class PlayerPaddle;
class GameItem;

class BallInPlayState : public GameState {

private:
	void DoBallCollision(GameBall& b, const Vector2D& n, float d);
	void DoItemCollision();

public:
	BallInPlayState(GameModel* gm);
	virtual ~BallInPlayState();

	virtual void Tick(double seconds);
	virtual void BallReleaseKeyPressed() {
		// Do nothing, ball is already in play.
	}

};
#endif