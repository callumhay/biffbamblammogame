/**
 * BallCamHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BallCamHUD.h"
#include "GameAssets.h"

#include "../GameModel/CannonBlock.h"

const float BallCamHUD::ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM = 150.0f;

BallCamHUD::BallCamHUD(GameAssets& assets) :
cannonRotateHint(assets.GetTutorialAssets(), "Rotate Cannon"),
cannonFireHint(assets.GetTutorialAssets(), "Fire Cannon"),
cannonCountdown(CannonBlock::BALL_CAMERA_ROTATION_TIME_IN_SECS), cannonHUDActive(false), cannon(NULL) {

    // Initialize the hints...
    std::list<GameViewConstants::XBoxButtonType> xboxButtonTypes;
    std::list<GameViewConstants::KeyboardButtonType> keyboardButtonTypes;
    std::list<std::string> buttonTexts;
    std::list<Colour> buttonColours;

    // Cannon rotation hint
    keyboardButtonTypes.clear();
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    buttonTexts.clear();
    buttonTexts.push_back("A");
    buttonTexts.push_back("D");

    this->cannonRotateHint.SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
    this->cannonRotateHint.SetKeyboardButtons(keyboardButtonTypes, buttonTexts);

    // Cannon fire hint
    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    this->cannonFireHint.SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    this->cannonFireHint.SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
}

BallCamHUD::~BallCamHUD() {

}

void BallCamHUD::ToggleCannonHUD(bool activate, const CannonBlock* cannon) {
    if (activate) {
        
        if (this->cannonHUDActive) {
            return;
        }

        assert(cannon != NULL);
        this->cannon = cannon;
        this->cannonRotateHint.Show(0.5, 2.0);
        this->cannonFireHint.Show(0.5, 2.0);
        this->cannonHUDActive = true;
        this->cannonCountdown.Reset();
    }
    else {

        if (!this->cannonHUDActive) {
            return;
        }

        this->cannon = NULL;
        this->cannonRotateHint.Unshow(0.0, 0.5);
        this->cannonFireHint.Unshow(0.0, 0.5);
        this->cannonHUDActive = false;
    }
}

void BallCamHUD::Reinitialize() {
    this->cannonRotateHint.Unshow(0.0, 0.0, true);
    this->cannonFireHint.Unshow(0.0, 0.0, true);
    this->cannonHUDActive = false;
    this->cannonCountdown.Reset();
}

void BallCamHUD::DrawCannonHUD(double dT, const Camera& camera) {
    if (this->cannonRotateHint.GetAlpha() <= 0.0f && (!this->cannonHUDActive || this->cannon == NULL)) {
        return;
    }

    this->cannonRotateHint.Tick(dT);
    this->cannonFireHint.Tick(dT);

    // Place the hint in the correct location on-screen
    float yPos = this->cannonRotateHint.GetHeight() + ROTATE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    this->cannonRotateHint.SetTopLeftCorner((camera.GetWindowWidth() - this->cannonRotateHint.GetWidth()) / 2.0f, yPos);
    this->cannonRotateHint.Draw(camera);

    yPos -= (this->cannonRotateHint.GetHeight() + 50);
    this->cannonFireHint.SetTopLeftCorner((camera.GetWindowWidth() - this->cannonFireHint.GetWidth()) / 2.0f, yPos);
    this->cannonFireHint.Draw(camera);

    if (!this->cannonHUDActive || this->cannon == NULL) {
        return;
    }

    this->cannonCountdown.DrawUsingTimeLeft(camera, dT, 
        this->cannon->GetTotalRotationTime() - this->cannon->GetElapsedRotationTime());
}