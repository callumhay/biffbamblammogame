/**
 * BallReleaseHUD.h
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

#ifndef __BALL_RELEASE_HUD_H__
#define __BALL_RELEASE_HUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"

#include "../GameSound/SoundCommon.h"

class Camera;
class GameModel;
class GameSound;

class BallReleaseHUD {
public:
    BallReleaseHUD(GameSound* sound);
    ~BallReleaseHUD();

    void Draw(double dT, const Camera& camera, const GameModel& gameModel);
    void TimerStarted();
    void BallReleased();
    void StopAndClearTimer();

    void Reinitialize();

private:
    enum State { NOT_STARTED = 0, STARTING, TICKING, ENDING, RELEASED };
    State currState;

    AnimationLerp<float> fadeAnim;
    AnimationMultiLerp<float> flashAnim;
    AnimationMultiLerp<float> scaleAnim;

    float lastKnownPercentElapsedAmt;
    Point2D lastKnownPaddlePos;

    GameSound* sound;
    SoundID runningOutOfTimeSoundID;

    void SetState(const BallReleaseHUD::State& state);
    void DrawReleaseTimer(const Camera& camera, const GameModel& gameModel, const Point2D& paddlePos,
        float scale, float alpha, float flashAmt, float percentElapsed);

    DISALLOW_COPY_AND_ASSIGN(BallReleaseHUD);
};

inline void BallReleaseHUD::TimerStarted() {
    this->SetState(BallReleaseHUD::STARTING);
}
inline void BallReleaseHUD::BallReleased() {
    this->SetState(BallReleaseHUD::RELEASED);
}
inline void BallReleaseHUD::StopAndClearTimer() {
    this->SetState(BallReleaseHUD::RELEASED);
}


#endif // __BALL_RELEASE_HUD_H__