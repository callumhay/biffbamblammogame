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
	GameItem* debugItemDrop;	// Item to drop if debug drop is needed.
	double timeSinceGhost;		// The time since the ball stopped colliding with blocks

	void DoBallCollision(GameBall& b, const Vector2D& n, float d);
	void DoItemCollision();
	void UpdateActiveTimers(double seconds);

public:
	BallInPlayState(GameModel* gm);
	virtual ~BallInPlayState();

	virtual void Tick(double seconds);
	virtual void BallReleaseKeyPressed() {
		// Do nothing, ball is already in play.
	}

	void DebugDropItem(GameItem* item);
};
#endif