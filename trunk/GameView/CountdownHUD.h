/**
 * CountdownHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __COUNTDOWNHUD_H__
#define __COUNTDOWNHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

class GameModel;
class Camera;

class CountdownHUD {
public:
    CountdownHUD(double totalTimeUntilCountdownOver);
    ~CountdownHUD();

    void Reset() { this->SetState(NotOn); }
    void SetTotalTimeUntilCountdownIsOver(double totalTime) { this->totalTimeUntilCountdownOver = totalTime; }

    void DrawUsingTimeLeft(const Camera& camera, double dT, double timeLeft);
    void DrawUsingTimeElapsed(const Camera& camera, double dT, double timeElapsed);

private:
    double totalTimeUntilCountdownOver;

    AnimationLerp<float> numFadeAnim;
    AnimationLerp<float> numScaleAnim;
    TextLabel2D numLabel;

    enum CountdownState { NotOn, ThreeSeconds, TwoSeconds, OneSecond };
    CountdownState currState;

    void SetState(CountdownState state);

    DISALLOW_COPY_AND_ASSIGN(CountdownHUD);
};

inline void CountdownHUD::DrawUsingTimeElapsed(const Camera& camera, double dT, double timeElapsed) {
    this->DrawUsingTimeLeft(camera, dT, this->totalTimeUntilCountdownOver - timeElapsed);
}

#endif // __COUNTDOWNHUD_H__