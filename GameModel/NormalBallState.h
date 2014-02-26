/**
 * NormalBallState.h
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

#ifndef __NORMALBALLSTATE_H__
#define __NORMALBALLSTATE_H__

#include "BallState.h"
#include "GameBall.h"

class GameModel;

// The state that the game ball is usually in - does its normal tick where the ball
// travels around the level based on its velocity
class NormalBallState : public BallState {
public:
	NormalBallState(GameBall* ball);
	~NormalBallState();

	BallState* Clone(GameBall* newBall) const;
	void Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel);
	ColourRGBA GetBallColour() const;

	BallState::BallStateType GetBallStateType() const;

private:
    static const double OMNI_BULLET_WAIT_TIME_IN_SECS;
    static const int MAX_BULLETS_AT_A_TIME;

    double timeSinceLastOmniBullets;
	bool ApplyCrazyBallVelocityChange(double dT, Vector2D& currVelocity, GameModel* gameModel);
    void ApplyGravityBallVelocityChange(double dT, const GameModel* gameModel, const Vector2D& worldSpaceGravityDir, Vector2D& currVelocity);
    void AttemptFireOfOmniBullets(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(NormalBallState);
};

// Return the typical ball colour
inline ColourRGBA NormalBallState::GetBallColour() const {
	return this->gameBall->contributingIceColour * this->gameBall->contributingCrazyColour * 
				 this->gameBall->contributingGravityColour * this->gameBall->colour;
}

inline BallState::BallStateType NormalBallState::GetBallStateType() const {
	return BallState::NormalBallState;
}

#endif // __NORMALBALLSTATE_H__