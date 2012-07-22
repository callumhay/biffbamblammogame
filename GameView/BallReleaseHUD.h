/**
 * BallReleaseHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALL_RELEASE_HUD_H__
#define __BALL_RELEASE_HUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"

class Camera;
class GameModel;

class BallReleaseHUD {
public:
    BallReleaseHUD();
    ~BallReleaseHUD();

    void Draw(double dT, const Camera& camera, const GameModel& gameModel);
    void TimerStarted();
    void BallReleased();

    void Reinitialize();

private:
    enum State { NOT_STARTED = 0, STARTING, TICKING, ENDING, RELEASED };
    State currState;

    AnimationLerp<float> fadeAnim;
    AnimationMultiLerp<float> flashAnim;
    AnimationMultiLerp<float> scaleAnim;

    float lastKnownPercentElapsedAmt;
    Point2D lastKnownPaddlePos;

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


#endif // __BALL_RELEASE_HUD_H__