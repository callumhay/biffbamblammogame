/**
 * BallReleaseHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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