/**
 * RemoteControlRocketHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
    if (!this->isActive || rocket == NULL) {
        return;
    }

    // Place the hint in the correct location on-screen
    float yPos = this->moveHint.GetHeight() + MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    this->moveHint.SetTopLeftCorner((camera.GetWindowWidth() - this->moveHint.GetWidth()) / 2.0f, yPos);
    this->moveHint.Tick(dT);
    this->moveHint.Draw(camera);

    yPos -= (this->moveHint.GetHeight() + 50);
    this->thrustHint.SetTopLeftCorner((camera.GetWindowWidth() - this->moveHint.GetWidth()) / 2.0f, yPos);
    this->thrustHint.Tick(dT);
    this->thrustHint.Draw(camera);

    this->rocketExplodeCountdown.Draw(camera, dT, 
        PaddleRemoteControlRocketProjectile::TIME_BEFORE_FUEL_RUNS_OUT_IN_SECS - this->rocket->GetTimeUntilFuelRunsOut());
}

void RemoteControlRocketHUD::Activate(const PaddleRemoteControlRocketProjectile* rocket) {
    if (this->isActive || rocket == NULL) {
        return;
    }
    this->moveHint.Show(0.5, 1.0);
    this->thrustHint.Show(0.5, 1.0);
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