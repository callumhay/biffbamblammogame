/**
 * BallInPlayState.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
class SafetyNet;
struct BallCollisionChangeInfo;

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
    void ShootActionContinuousUse(double dT, float magnitudePercent);
    void MoveKeyPressedForPaddle(int dir, float magnitudePercent);
    void MoveKeyPressedForOther(int dir, float magnitudePercent);
    void BallBoostDirectionPressed(float x, float y);
    void BallBoostDirectionReleased();
    void BallBoostReleasedForBall(const GameBall& ball);

	void Tick(double seconds);

	void DebugDropItem(GameItem* item);

private:
	double timeSinceGhost;		// The time since the ball stopped colliding with blocks

	void DoBallCollision(GameBall& b, const Vector2D& n, double dT, double timeUntilCollision, 
        float minAngleInDegs, const Vector2D& lineVelocity = Vector2D(0,0));
    void DoBallPaddleCollision(GameBall& b, PlayerPaddle& p, const Vector2D& n, double dT, double timeUntilCollision, 
        float minAngleInDegs, const Vector2D& lineVelocity);
	void DoBallCollision(GameBall& ball1, GameBall& ball2, const Point2D& ball1PtOfCollision, 
        const Point2D& ball2PtOfCollision, double dT, double timeUntilCollision);
	void DoItemCollision();
    void DoBallSafetyNetCollision(SafetyNet* safetyNet, GameBall& ball, double seconds, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& collisionPt,
        BallCollisionChangeInfo& collisionChangeInfo);

	bool AugmentBallDirectionToBeNotTooDownwards(GameBall& b, const PlayerPaddle& p, const Vector2D& collisionNormal);

	//void UpdateActiveBalls(double seconds);
    DISALLOW_COPY_AND_ASSIGN(BallInPlayState);
};

#endif