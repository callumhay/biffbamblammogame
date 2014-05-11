/**
 * InCannonBallState.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __INCANNONBALLSTATE_H__
#define __INCANNONBALLSTATE_H__

#include "BallState.h"

class CannonBlock;
class GameModel;

/**
 * Ball state that occurs when the ball has been lodged inside a cannon block and
 * is rotating inside the block preparing to fire in some random direction.
 * The state transition for this is typically:
 * NormalBallState -> InCannonBallState -> NormalBallState
 */
class InCannonBallState : public BallState {
public:
	InCannonBallState(GameBall* ball, CannonBlock* cannonBlock, BallState* prevState);
	~InCannonBallState();

	BallState* Clone(GameBall* newBall) const;
	void Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel);
	ColourRGBA GetBallColour() const;
	CannonBlock* GetCannonBlock() const;

	BallState::BallStateType GetBallStateType() const;

	BallState* GetPreviousState() const;

private:
	CannonBlock* cannonBlock;
	BallState* previousState;

};

// The ball is invisible while in the cannon
inline ColourRGBA InCannonBallState::GetBallColour() const {
	return ColourRGBA(0, 0, 0, 0);
}

inline CannonBlock* InCannonBallState::GetCannonBlock() const {
	return this->cannonBlock;
}

inline BallState::BallStateType InCannonBallState::GetBallStateType() const {
	return BallState::InCannonBallState;
}

inline BallState* InCannonBallState::GetPreviousState() const {
	return this->previousState;
}

#endif // __INCANNONBALLSTATE_H__