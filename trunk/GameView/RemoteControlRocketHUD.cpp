/**
 * RemoteControlRocketHUD.cpp
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

#include "RemoteControlRocketHUD.h"
#include "GameAssets.h"

#include "../GameModel/PaddleRemoteControlRocketProjectile.h"

#include "../BlammoEngine/Camera.h"

const float RemoteControlRocketHUD::MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM = 150.0f;

RemoteControlRocketHUD::RemoteControlRocketHUD(GameAssets& assets) : 
moveHint(assets.GetTutorialAssets(), "Movement"), 
thrustHint(assets.GetTutorialAssets(), "Thrust"),
isActive(false), rocketExplodeCountdown(PaddleRemoteControlRocketProjectile::TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS) {
    
    std::list<GameViewConstants::XBoxButtonType> xboxButtonTypes;
    std::list<GameViewConstants::KeyboardButtonType> keyboardButtonTypes;
    std::list<std::string> buttonTexts;
    std::list<Colour> buttonColours;

    // Initialize the hints...
    
    // Rocket movement hint
    this->moveHint.SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
    keyboardButtonTypes.clear();
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    buttonTexts.clear();
    buttonTexts.push_back("A");
    buttonTexts.push_back("D");
    
    this->moveHint.SetKeyboardButtons(keyboardButtonTypes, buttonTexts);

    // Rocket thrust hint
    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    this->thrustHint.SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    this->thrustHint.SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
}

RemoteControlRocketHUD::~RemoteControlRocketHUD() {
}

void RemoteControlRocketHUD::Draw(double dT, const Camera& camera) {
    if (this->moveHint.GetAlpha() <= 0.0f && (!this->isActive || this->rocket == NULL)) {
        return;
    }

    this->moveHint.Tick(dT);
    this->thrustHint.Tick(dT);

    // Place the hint in the correct location on-screen
    float yPos = this->moveHint.GetHeight() + MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    this->moveHint.SetTopLeftCorner((camera.GetWindowWidth() - this->moveHint.GetWidth()) / 2.0f, yPos);
    this->moveHint.Draw(camera);

    yPos -= (this->moveHint.GetHeight() + 50);
    this->thrustHint.SetTopLeftCorner((camera.GetWindowWidth() - this->thrustHint.GetWidth()) / 2.0f, yPos);
    this->thrustHint.Draw(camera);

    if (!this->isActive || this->rocket == NULL) {
        return;
    }

    this->rocketExplodeCountdown.DrawUsingTimeElapsed(camera, dT, 
        PaddleRemoteControlRocketProjectile::TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS - this->rocket->GetTimeUntilFuelRunsOut());
}

void RemoteControlRocketHUD::Activate(const PaddleRemoteControlRocketProjectile* rocket) {
    if (this->isActive || rocket == NULL) {
        return;
    }
    this->moveHint.Show(0.5, 2.0);
    this->thrustHint.Show(0.5, 2.0);
    this->isActive = true;
    this->rocket = rocket;
    this->rocketExplodeCountdown.Reset();
}

void RemoteControlRocketHUD::Deactivate() {
    if (!this->isActive) {
        return;
    }
    this->moveHint.Unshow(0.0, 0.5);
    this->thrustHint.Unshow(0.0, 0.5);
    this->isActive = false;
    this->rocket = NULL;
}

void RemoteControlRocketHUD::Reinitialize() {
    this->moveHint.Unshow(0.0, 0.0, true);
    this->thrustHint.Unshow(0.0, 0.0, true);
    this->isActive = false;
    this->rocket   = NULL;
    this->rocketExplodeCountdown.Reset();
}