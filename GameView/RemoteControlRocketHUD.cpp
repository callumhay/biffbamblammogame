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

#include "../BlammoEngine/Camera.h"

const float RemoteControlRocketHUD::MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM = 150.0f;

RemoteControlRocketHUD::RemoteControlRocketHUD(GameAssets& assets) : 
moveHint(assets.GetTutorialAssets(), "Rocket Control"), isActive(false) {
    
    std::list<GameViewConstants::KeyboardButtonType> keyboardButtonTypes;
    std::list<std::string> buttonTexts;

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

    // TODO: Rocket detonate hint?
}

RemoteControlRocketHUD::~RemoteControlRocketHUD() {
}

void RemoteControlRocketHUD::Draw(double dT, const Camera& camera) {
    if (!this->isActive) {
        return;
    }

    // Place the hint in the correct location on-screen
    this->moveHint.SetTopLeftCorner((camera.GetWindowWidth() - this->moveHint.GetWidth()) / 2.0f, 
        this->moveHint.GetHeight() + MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM);
    this->moveHint.Tick(dT);
    this->moveHint.Draw(camera);
}

void RemoteControlRocketHUD::Activate() {
    if (this->isActive) {
        return;
    }
    this->moveHint.Show(0.5, 1.0);
    this->isActive = true;
}

void RemoteControlRocketHUD::Deactivate() {
    if (!this->isActive) {
        return;
    }
    this->moveHint.Unshow(0.0, 0.5);
    this->isActive = false;
}

void RemoteControlRocketHUD::Reinitialize() {
    this->moveHint.Unshow(0.0, 0.0, true);
    this->isActive = false;
}