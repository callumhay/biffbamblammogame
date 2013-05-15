/**
 * BoostCountdownHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOOSTCOUNTDOWNHUD_H__
#define __BOOSTCOUNTDOWNHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Animation.h"

class GameModel;
class Camera;

class BoostCountdownHUD {
public:
    BoostCountdownHUD();
    ~BoostCountdownHUD();

    void Reset() { this->SetState(NotOn); }
    void Draw(const Camera& camera, const GameModel& gameModel, double dT);

private:
    AnimationLerp<float> numFadeAnim;
    AnimationLerp<float> numScaleAnim;
    TextLabel2D numLabel;

    enum CountdownState { NotOn, ThreeSeconds, TwoSeconds, OneSecond };
    CountdownState currState;

    void SetState(CountdownState state);

    DISALLOW_COPY_AND_ASSIGN(BoostCountdownHUD);
};

#endif // __BOOSTCOUNTDOWNHUD_H__