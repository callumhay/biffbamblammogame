#ifndef __BALL_IN_PLAY_STATE_H__
#define __BALL_IN_PLAY_STATE_H__

#include <vector>

#include "GameState.h"

class GameBall;
class Vector2D;
class PlayerPaddle;
class GameItem;
class Point2D;

namespace Collision {
class LineSeg2D;
}

class BallInPlayState : public GameState {

private:
	double timeSinceGhost;		// The time since the ball stopped colliding with blocks

	bool IsOutOfGameBounds(const Point2D& pos);

	void DoBallCollision(GameBall& b, const Vector2D& n, Collision::LineSeg2D& collisionLine, double dT, double timeSinceCollision);
	void DoBallCollision(GameBall& ball1, GameBall& ball2);
	void DoItemCollision();
	
	//void UpdateActiveBalls(double seconds);
	void UpdateActiveTimers(double seconds);
	void UpdateActiveItemDrops(double seconds);
	void UpdateActiveProjectiles(double seconds);
	void UpdateActiveBeams(double seconds);

public:
	BallInPlayState(GameModel* gm);
	~BallInPlayState();

	GameState::GameStateType GetType() const {
		return GameState::BallInPlayStateType;
	}

	void Tick(double seconds);
	void BallReleaseKeyPressed();
	void DebugDropItem(GameItem* item);
};
#endif