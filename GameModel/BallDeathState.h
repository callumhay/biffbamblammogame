/**
 * BallDeathState.h
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

#ifndef __BALLDEATHSTATE_H__
#define __BALLDEATHSTATE_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"

#include "GameState.h"

class GameModel;
class GameBall;

/**
 * State for when the last ball goes out of bounds and is about to be destroyed -
 * this state allows for some time to show a death animation or some such thing
 * before the game state changes back to either the ball being on the paddle or
 * game over.
 */
class BallDeathState : public GameState {	
public:
	BallDeathState(GameBall* lastBallToBeAlive, GameModel* gm);
	~BallDeathState();

	GameState::GameStateType GetType() const {
		return GameState::BallDeathStateType;
	}

    void ShootActionReleaseUse();
    void MoveKeyPressedForPaddle(int dir, float magnitudePercent) {
        UNUSED_PARAMETER(dir);
        UNUSED_PARAMETER(magnitudePercent);
    }

	void Tick(double seconds);

private:
	// Constant Ball spiraling death values
	static const double SPIRAL_ANIMATION_TIME_TOTAL;
	static const double EXPLOSION_ANIMATION_TIME_TOTAL;
	static const double BALL_FADE_TIME_TOTAL;

	// Ball starts off by spiralling to its death, then it explodes and is officially dead afterwards
	enum DeathAnimationState { SpiralingToDeath, Exploding, Dead };
	DeathAnimationState currAnimationState;

	// Values related to the last ball - which is inevidbly going to die
	GameBall* lastBallToBeAlive;
	Point2D initialBallPosition;
	Vector2D initialBallVelocityDir;
	float initialBallVelocityMag;

	// Varying Ball spiraling death values
	float spiralRadius;
	AnimationLerp<float> spiralAnimation;
	double timeElapsed;

	void ExecuteSpiralingToDeathState(double dT);
};

#endif // __BALLDEATHSTATE_H__