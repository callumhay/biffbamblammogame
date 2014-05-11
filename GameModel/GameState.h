/**
 * GameState.h
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

#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class GameBall;

class GameState {
public:
	virtual ~GameState() {}

	enum GameStateType { NULLStateType, BallDeathStateType, BallInPlayStateType, BallOnPaddleStateType, 
                         BallWormholeStateType, GameCompleteStateType, GameOverStateType, 
                         LevelStartStateType, LevelCompleteStateType, WorldCompleteStateType };

    static GameState* Build(const GameState::GameStateType& stateType, GameModel* gameModel);

    static bool IsGameInPlayState(const GameModel& gameModel);

	virtual GameState::GameStateType GetType() const = 0;
	virtual void Tick(double seconds) = 0;
    virtual void ShootActionReleaseUse() = 0;
    virtual void ShootActionContinuousUse(double dT, float magnitudePercent) { UNUSED_PARAMETER(dT); UNUSED_PARAMETER(magnitudePercent); }
	virtual void MoveKeyPressedForPaddle(int dir, float magnitudePercent);
    virtual void MoveKeyPressedForOther(int dir, float magnitudePercent) { UNUSED_PARAMETER(dir); UNUSED_PARAMETER(magnitudePercent); }
    virtual void BallBoostDirectionPressed(float x, float y);
    virtual void BallBoostDirectionReleased();
    virtual void BallBoostReleasedForBall(const GameBall& ball);

	bool DoUpdateToPaddleBoundriesAndCollisions(double dT, bool doAttachedBallCollision);

protected:
    GameState(GameModel* gm) : gameModel(gm) {}

	GameModel* gameModel;

private:
    DISALLOW_COPY_AND_ASSIGN(GameState);
};

inline void GameState::BallBoostDirectionPressed(float x, float y) {
    UNUSED_PARAMETER(x);
    UNUSED_PARAMETER(y);
    // Do nothing by default
}

inline void GameState::BallBoostDirectionReleased() {
    // Do nothing by default
}

inline void GameState::BallBoostReleasedForBall(const GameBall&) {
    // Do nothing by default
}

#endif