/**
 * BallInPlayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BALL_IN_PLAY_STATE_H__
#define __BALL_IN_PLAY_STATE_H__

#include <vector>
#include "../BlammoEngine/Vector.h"

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

public:
	BallInPlayState(GameModel* gm);
	~BallInPlayState();

	GameState::GameStateType GetType() const {
		return GameState::BallInPlayStateType;
	}

	void ShootActionReleaseUse();
    void ShootActionContinuousUse(float magnitudePercent);
    void MoveKeyPressedForPaddle(int dir, float magnitudePercent);
    void MoveKeyPressedForOther(int dir, float magnitudePercent);
    void BallBoostDirectionPressed(float x, float y);
    void BallBoostDirectionReleased();
    void BallBoostReleasedForBall(const GameBall& ball);

	void Tick(double seconds);

	void DebugDropItem(GameItem* item);

private:
	double timeSinceGhost;		// The time since the ball stopped colliding with blocks

	void DoBallCollision(GameBall& b, const Vector2D& n, Collision::LineSeg2D& collisionLine,
        double dT, double timeUntilCollision, float minAngleInDegs, const Vector2D& lineVelocity = Vector2D(0,0), bool paddleReflection = false);
	void DoBallCollision(GameBall& ball1, GameBall& ball2);
	void DoItemCollision();

	bool AugmentBallDirectionToBeNotTooDownwards(GameBall& b, const PlayerPaddle& p, const Vector2D& collisionNormal);
	
	//void UpdateActiveBalls(double seconds);
    DISALLOW_COPY_AND_ASSIGN(BallInPlayState);
};

#endif