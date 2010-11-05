/**
 * BallInPlayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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

	void AugmentBallDirectionToBeNotDownwards(GameBall& b);
	
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

// Makes sure that the ball's direction is upwards and changes it to be upwards - when it gets
// hit by the paddle this is called.
inline void BallInPlayState::AugmentBallDirectionToBeNotDownwards(GameBall& b) {
	Vector2D ballDirection = b.GetDirection();

	if (ballDirection[1] < 0) {
		ballDirection[1] = fabs(ballDirection[0] * tanf(GameBall::MIN_BALL_ANGLE_IN_RADS));
		ballDirection.Normalize();
		b.SetDirection(ballDirection);
	}
}

#endif