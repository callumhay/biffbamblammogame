/**
 * BoostCountdownHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BoostCountdownHUD.h"
#include "GameFontAssetsManager.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/BallBoostModel.h"
#include "../BlammoEngine/Camera.h"

BoostCountdownHUD::BoostCountdownHUD() :
numLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
GameFontAssetsManager::Huge), "3"),
currState(NotOn) {

    this->numLabel.SetColour(Colour(1,1,1));
    this->numLabel.SetDropShadow(Colour(0,0,0), 0.065f);

    this->numFadeAnim.ClearLerp();
    this->numFadeAnim.SetInterpolantValue(0.0f);
    this->numFadeAnim.SetRepeat(false);
    this->numScaleAnim.ClearLerp();
    this->numScaleAnim.SetInterpolantValue(1.0f);
    this->numScaleAnim.SetRepeat(false);
}

BoostCountdownHUD::~BoostCountdownHUD() {
}

void BoostCountdownHUD::Draw(const Camera& camera, const GameModel& gameModel, double dT) {

    const BallBoostModel* boostModel = gameModel.GetBallBoostModel();
    if (boostModel == NULL) {
        return;
    }

    // If the boost model is in bullet time we check the countdown towards the end of bullet time
    // when it reaches 3, 2 and 1 seconds left, we create a text label and display it for each
    if (boostModel->GetBulletTimeState() == BallBoostModel::BulletTime) {
        double timeLeft = BallBoostModel::GetMaxBulletTimeDuration() - boostModel->GetTotalBulletTimeElapsed();
        
        if (timeLeft > 3.0) {
            this->SetState(NotOn);
            return;
        }

        switch (this->currState) {
            case NotOn:
                if (timeLeft <= 3.0) {
                    this->SetState(ThreeSeconds);
                }
                break;

            case ThreeSeconds:
                if (timeLeft <= 2.0) {
                    this->SetState(TwoSeconds);
                }
                break;
            
            case TwoSeconds:
                if (timeLeft <= 1.0) {
                    this->SetState(OneSecond);
                }
                break;

            case OneSecond:
                break;

            default:
                assert(false);
                return;
        }

        // Draw the current number label
        this->numLabel.SetAlpha(this->numFadeAnim.GetInterpolantValue());
        this->numLabel.SetScale(this->numScaleAnim.GetInterpolantValue());
        this->numLabel.SetTopLeftCorner((camera.GetWindowWidth() - this->numLabel.GetLastRasterWidth()) / 2.0f,
                                        (camera.GetWindowHeight() + this->numLabel.GetHeight()) / 2.0f);
        this->numLabel.Draw();

        this->numFadeAnim.Tick(dT);
        this->numScaleAnim.Tick(dT);
    }
    else {
        this->SetState(NotOn);
    }
}


void BoostCountdownHUD::SetState(CountdownState state) {
    static const float MIN_SCALE_AMOUNT      = 0.2f;
    static const float MAX_SCALE_AMOUNT      = 3.0f;
    static const double LERP_TIME_IN_SECONDS = 0.85;
    static const float END_FADE_AMOUNT       = 0.1f;

    switch (state) {
        case NotOn:
            this->numFadeAnim.ClearLerp();
            this->numFadeAnim.SetInterpolantValue(0.0f);
            this->numScaleAnim.ClearLerp();
            this->numScaleAnim.SetInterpolantValue(1.0f);
            break;

        case ThreeSeconds:
            this->numLabel.SetText("3");
            this->numFadeAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, 1.0f, END_FADE_AMOUNT);
            this->numFadeAnim.SetInterpolantValue(1.0f);
            this->numScaleAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, MIN_SCALE_AMOUNT, MAX_SCALE_AMOUNT);
            this->numScaleAnim.SetInterpolantValue(MIN_SCALE_AMOUNT);
            break;

        case TwoSeconds:
            this->numLabel.SetText("2");
            this->numFadeAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, 1.0f, END_FADE_AMOUNT);
            this->numFadeAnim.SetInterpolantValue(1.0f);
            this->numScaleAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, MIN_SCALE_AMOUNT, MAX_SCALE_AMOUNT);
            this->numScaleAnim.SetInterpolantValue(MIN_SCALE_AMOUNT);
            break;

        case OneSecond:
            this->numLabel.SetText("1");
            this->numFadeAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, 1.0f, END_FADE_AMOUNT);
            this->numFadeAnim.SetInterpolantValue(1.0f);
            this->numScaleAnim.SetLerp(0.0, LERP_TIME_IN_SECONDS, MIN_SCALE_AMOUNT, MAX_SCALE_AMOUNT);
            this->numScaleAnim.SetInterpolantValue(MIN_SCALE_AMOUNT);
            break;

        default:
            assert(false);
            return;
    }
    this->currState = state;
}