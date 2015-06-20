#include "ArcadeController.h"
#include "ArcadeControllerEventsListener.h"
#include "ArcadeSerialComm.h"

#include "../GameModel/GameModel.h"
#include "../GameView/GameDisplay.h"
#include "../GameView/GameViewEventManager.h"

ArcadeController::ArcadeController(GameModel* model, GameDisplay* display) : 
KeyboardSDLController(model, display), eventsListener(NULL), 
serialComm(new ArcadeSerialComm()), joystick(NULL), joystickX(0), joystickY(0), fireKey(SDLK_SPACE), boostKey(SDLK_RETURN) {

    this->eventsListener = new ArcadeControllerEventsListener(display, *this->serialComm);

    this->leftActionOn = this->rightActionOn = this->upActionOn =
        this->downActionOn = this->specialDirOn = false;
    this->directionMagnitudePercentLeftRight = this->directionMagnitudePercentUpDown = 0;

    this->joystick = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);

    GameEventManager::Instance()->RegisterGameEventListener(this->eventsListener);
    GameViewEventManager::Instance()->RegisterListener(this->eventsListener);
}

ArcadeController::~ArcadeController() {
    if (this->joystick != NULL) {
        SDL_JoystickClose(this->joystick);
        this->joystick = NULL;
    }
    SDL_JoystickEventState(SDL_DISABLE);

    GameEventManager::Instance()->UnregisterGameEventListener(this->eventsListener);
    GameViewEventManager::Instance()->UnregisterListener(this->eventsListener);

    delete this->eventsListener;
    this->eventsListener = NULL;
    delete this->serialComm;
    this->serialComm = NULL;
}

bool ArcadeController::ProcessState(double dT) {
    UNUSED_PARAMETER(dT);

    SDL_Event keyEvent;
    // Grab all the events off the queue
    while (SDL_PollEvent(&keyEvent)) {
        switch (keyEvent.type) {

            case SDL_ACTIVEEVENT:
                this->WindowActiveEvent(keyEvent.active);
                break;

            case SDL_JOYAXISMOTION:
                this->JoystickMotion(keyEvent.jaxis);
                break;

            case SDL_KEYDOWN:
                this->KeyDown(keyEvent.key.keysym.sym);
                break;

            case SDL_KEYUP:
                this->KeyUp(keyEvent.key.keysym.sym);
                break;

            case SDL_QUIT:
                return true;

            default:
                break;
        }
    }

    return false;
}

void ArcadeController::Sync(size_t frameID, double dT) {

    // Paddle controls (NOTE: the else is to make the feedback more exact)
    if (this->leftActionOn || this->keyPressed[SDLK_LEFT] || this->keyPressed[SDLK_a]) {

        int leftDir = this->model->AreControlsFlippedForPaddle() ? 1 : -1;
        this->model->MovePaddle(frameID, leftDir, this->directionMagnitudePercentLeftRight);
        leftDir = this->model->AreControlsFlippedForOther() ? 1 : -1;
        this->model->MoveOther(frameID, leftDir, this->directionMagnitudePercentLeftRight);
    }
    else if (this->rightActionOn || this->keyPressed[SDLK_RIGHT] || this->keyPressed[SDLK_d]) {

        int rightDir = this->model->AreControlsFlippedForPaddle() ? -1 : 1;
        this->model->MovePaddle(frameID, rightDir, this->directionMagnitudePercentLeftRight);
        rightDir = this->model->AreControlsFlippedForOther() ? -1 : 1;
        this->model->MoveOther(frameID, rightDir, this->directionMagnitudePercentLeftRight);
    }
    else {
        this->model->MovePaddle(frameID, 0, 0.0);
        this->model->MoveOther(frameID, 0, 0.0);
    }

    /*
    // ****NOTE: IMPLEMENT ME IF DOING VIBRATION!!!
    // Check for vibration time expiration...
    if (this->vibrateLengthInSeconds > 0.0) {
        if (this->vibrateTimeTracker >= this->vibrateLengthInSeconds) {
            this->vibrateTimeTracker     = 0.0;
            this->vibrateLengthInSeconds = 0.0;
            this->SetVibration(BBBGameController::NoVibration, BBBGameController::NoVibration);
        }
        else {
            this->vibrateTimeTracker += dT;
        }
    }
    */

    KeyboardSDLController::Sync(frameID, dT);
}

void ArcadeController::SetSerialPort(const std::string& serialPort) {
    this->serialComm->OpenSerial(serialPort);
}

void ArcadeController::JoystickMotion(const SDL_JoyAxisEvent& joyAxisEvent) {
    if (this->joystick == NULL || joyAxisEvent.which != SDL_JoystickIndex(this->joystick)) {
        return;
    }
    assert(SDL_JoystickNumAxes(this->joystick) >= 2);

    Sint16 sensitivityX = 5000;
    Sint16 sensitivityY = 4000;

    bool isInGameState = DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState());

    // Basic game pad buttons - functionality depends on whether we're in-game or not...
    if (!isInGameState) {
        sensitivityX = 30000;
        sensitivityY = 10000;
    }

    if (joyAxisEvent.axis == 0) {
        this->joystickX = abs(joyAxisEvent.value) >= sensitivityX ? joyAxisEvent.value : 0;
        int tempY = SDL_JoystickGetAxis(this->joystick, 1);
        this->joystickY = abs(tempY) >= sensitivityY ? tempY : 0;
    }
    else {
        this->joystickY = abs(joyAxisEvent.value) >= sensitivityY ? joyAxisEvent.value : 0;
        int tempX = SDL_JoystickGetAxis(this->joystick, 0);
        this->joystickX = abs(tempX) >= sensitivityX ? tempX : 0;
    }

    int absJoystickX = abs(this->joystickX);
    int absJoystickY = abs(this->joystickY);

    if ((absJoystickY >= sensitivityY && this->joystickY < 0)) {

        if (!this->upActionOn && !this->downActionOn) {
            GameControl::ActionMagnitude magnitude = 
                ArcadeController::GetMagnitudeForJoystick(this->joystickY);

            // Only do the button press if the magnitude is greater in the y-axis than the x
            if (absJoystickY >= absJoystickX) {
                this->display->ButtonPressed(GameControl::UpButtonAction, magnitude);
            }
            this->upActionOn = true;
        }

        if (this->upActionOn) {
            this->directionMagnitudePercentUpDown = 
                static_cast<float>(absJoystickY - sensitivityY) /
                static_cast<float>(std::numeric_limits<Sint16>::max() - sensitivityY);
        }
    }
    else {
        if (this->upActionOn) {
            this->display->ButtonReleased(GameControl::UpButtonAction);
            this->upActionOn = false;
            this->directionMagnitudePercentUpDown = 0.0f;
        }
    }

    if (absJoystickY >= sensitivityY && this->joystickY > 0) {

        if (!this->downActionOn && !this->upActionOn) {
            GameControl::ActionMagnitude magnitude = 
                ArcadeController::GetMagnitudeForJoystick(this->joystickY);
   
            // Only do the button press if the magnitude is greater in the y-axis than the x
            if (absJoystickY >= absJoystickX) {
                this->display->ButtonPressed(GameControl::DownButtonAction, magnitude);
            }

            this->downActionOn = true;
        }

        if (this->downActionOn) {
            this->directionMagnitudePercentUpDown = 
                static_cast<float>(absJoystickY - sensitivityY) /
                static_cast<float>(std::numeric_limits<Sint16>::max() - sensitivityY);
        }
    }
    else {
        if (this->downActionOn) {
            this->display->ButtonReleased(GameControl::DownButtonAction);
            this->downActionOn = false;
            this->directionMagnitudePercentUpDown = 0.0f;
        }
    }

    if (absJoystickX >= sensitivityX && this->joystickX < 0) {

        if (!this->leftActionOn && !this->rightActionOn) {

            GameControl::ActionMagnitude magnitude = 
                ArcadeController::GetMagnitudeForJoystick(this->joystickX);

            // We do a button pressed only if the magnitude is greater on the x-axis than the y
            if (absJoystickX >= absJoystickY) {
                this->display->ButtonPressed(GameControl::LeftButtonAction, magnitude);
            }

            this->leftActionOn = true;
        }

        if (this->leftActionOn) {
            this->directionMagnitudePercentLeftRight = 
                static_cast<float>(absJoystickX - sensitivityX) /
                static_cast<float>(std::numeric_limits<Sint16>::max() - sensitivityX);
        }
    }
    else {
        if (this->leftActionOn) {
            this->display->ButtonReleased(GameControl::LeftButtonAction);
            this->leftActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
        }
    }

    if (absJoystickX >= sensitivityX && this->joystickX > 0) {

        if (!this->rightActionOn && !this->leftActionOn) {

            GameControl::ActionMagnitude magnitude = 
                ArcadeController::GetMagnitudeForJoystick(this->joystickX);

            // We do a button pressed only if the magnitude is greater on the x-axis than the y
            if (absJoystickX >= absJoystickY) {
                this->display->ButtonPressed(GameControl::RightButtonAction, magnitude);
            }

            this->rightActionOn = true;
        }

        if (this->rightActionOn) {
            this->directionMagnitudePercentLeftRight = 
                static_cast<float>(absJoystickX - sensitivityX) /
                static_cast<float>(std::numeric_limits<Sint16>::max() - sensitivityX);
        }
    }
    else {
        if (this->rightActionOn) {
            this->display->ButtonReleased(GameControl::RightButtonAction);
            this->rightActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
        }
    }

    // Special stuff (ball bullet time)...
    if (DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState())) {
        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel == NULL) {
            this->specialDirOn = false;
        }
        else {
            if (boostModel->IsInBulletTime()) {
                if (this->joystickX != 0 || this->joystickY != 0) {
                    this->display->SpecialDirectionPressed(this->joystickX, -this->joystickY);
                    this->specialDirOn = true;
                }
            }
            else {
                if (this->specialDirOn) {
                    this->display->SpecialDirectionReleased();
                    this->specialDirOn = false;
                }
            }
        }
    }
    else {
        this->specialDirOn = false;
    }
}

void ArcadeController::KeyDown(SDLKey key) {
    if (key < 0 || key > SDLK_LAST) { return; }

    // Determine if the "fire" or "boost" buttons are being pressed, 
    // override the super class if this is the case
    if (key == this->fireKey) {
        this->SetKeyPress(key, true);                                                      
        this->model->ShootActionReleaseUse();
        this->display->ButtonPressed(GameControl::EnterButtonAction);
        return;
    }
    else if (key == this->boostKey) {
        this->SetKeyPress(key, true);

        if (DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState())) {
            const BallBoostModel* boostModel = this->model->GetBallBoostModel();
            if (boostModel == NULL) {
                return;
            }
            if (boostModel->IsInBulletTime()) {
                this->display->SpecialDirectionReleased();
            }
            else {
                Vector2D boostDir(this->joystickX, -this->joystickY);
                if (boostDir.IsZero()) {
                    boostDir[0] = 1;
                }
                this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
                this->specialDirOn = true;
            }
        }
        else {
            this->display->ButtonPressed(GameControl::EnterButtonAction);
        }
        return;
    }

    KeyboardSDLController::KeyDown(key);
}

void ArcadeController::KeyUp(SDLKey key) {
    if (key < 0 || key > SDLK_LAST) { return; }

    // Determine if the "fire" or "boost" buttons are being released, 
    // override the super class if this is the case
    if (key == this->fireKey) {
        this->SetKeyPress(key, false);
        return;
    }
    else if (key == this->boostKey) {
        this->SetKeyPress(key, false);

        if (DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState())) {
            const BallBoostModel* boostModel = this->model->GetBallBoostModel();
            if (boostModel == NULL) {
                return;
            }
            this->display->SpecialDirectionReleased();
            this->specialDirOn = false;
        }
        return;
    }

    KeyboardSDLController::KeyUp(key);
}

GameControl::ActionMagnitude ArcadeController::GetMagnitudeForJoystick(Sint16 m) {
    const int SMALL_MAGNITUDE_MAX_VALUE  = 10000;
    const int NORMAL_MAGNITUDE_MAX_VALUE = SMALL_MAGNITUDE_MAX_VALUE  + 8000;
    const int LARGE_MAGNITUDE_MAX_VALUE  = NORMAL_MAGNITUDE_MAX_VALUE + 7000;

    int absValue = abs(m);

    if (absValue == 0) {
        return GameControl::ZeroMagnitude;
    }
    else if (absValue <= SMALL_MAGNITUDE_MAX_VALUE) {
        return GameControl::SmallMagnitude;
    }
    else if (absValue <= NORMAL_MAGNITUDE_MAX_VALUE) {
        return GameControl::NormalMagnitude;
    }
    else if (absValue <= LARGE_MAGNITUDE_MAX_VALUE) {
        return GameControl::LargeMagnitude;
    }

    return GameControl::FullMagnitude;
}