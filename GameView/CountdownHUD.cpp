/**
 * CountdownHUD.cpp
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

#include "CountdownHUD.h"
#include "GameFontAssetsManager.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/BallBoostModel.h"
#include "../BlammoEngine/Camera.h"

CountdownHUD::CountdownHUD(double totalTimeUntilCountdownOver) :
numLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
GameFontAssetsManager::Huge), "3"),
currState(NotOn), totalTimeUntilCountdownOver(totalTimeUntilCountdownOver) {
    
    assert(totalTimeUntilCountdownOver >= 0.0);

    this->numLabel.SetColour(Colour(1,1,1));
    this->numLabel.SetDropShadow(Colour(0,0,0), 0.065f);

    this->numFadeAnim.ClearLerp();
    this->numFadeAnim.SetInterpolantValue(0.0f);
    this->numFadeAnim.SetRepeat(false);
    this->numScaleAnim.ClearLerp();
    this->numScaleAnim.SetInterpolantValue(1.0f);
    this->numScaleAnim.SetRepeat(false);

    this->Reset();
}

CountdownHUD::~CountdownHUD() {
}

void CountdownHUD::DrawUsingTimeLeft(const Camera& camera, double dT, double timeLeft) {

    UNUSED_PARAMETER(camera);

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
    if (this->numFadeAnim.GetInterpolantValue() > 0.0f) {
        this->numLabel.SetAlpha(this->numFadeAnim.GetInterpolantValue());
        this->numLabel.SetScale(this->numScaleAnim.GetInterpolantValue());
        this->numLabel.SetTopLeftCorner((Camera::GetWindowWidth() - this->numLabel.GetLastRasterWidth()) / 2.0f,
                                        (Camera::GetWindowHeight() + this->numLabel.GetHeight()) / 2.0f);
        this->numLabel.Draw();
    }

    this->numFadeAnim.Tick(dT);
    this->numScaleAnim.Tick(dT);
}

void CountdownHUD::SetState(CountdownState state) {
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