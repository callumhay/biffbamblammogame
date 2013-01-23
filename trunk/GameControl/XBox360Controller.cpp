/**
 * XBox360Controller.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "XBox360Controller.h"

#ifdef _WIN32

#include "GameControl.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameDisplay.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

int XBox360Controller::sensitivity = XBox360Controller::DEFAULT_SENSITIVITY;

static WORD GetXBoxVibrationAmtFromEnum(const BBBGameController::VibrateAmount& vibeAmt) {
	switch (vibeAmt) {
		case BBBGameController::NoVibration:
			return 0;
		case BBBGameController::VerySoftVibration:
			return static_cast<WORD>(1.0f/5.0f * MAXWORD);
		case BBBGameController::SoftVibration:
			return static_cast<WORD>(2.0f/5.0f * MAXWORD);
		case BBBGameController::MediumVibration:
			return static_cast<WORD>(3.0f/5.0f * MAXWORD);
		case BBBGameController::HeavyVibration:
			return static_cast<WORD>(4.0f/5.0f * MAXWORD);
		case BBBGameController::VeryHeavyVibration:
			return MAXWORD;
		default:
			assert(false);
			break;
	}

	return 0;
}

XBox360Controller::XBox360Controller(GameModel* model, GameDisplay* display, int controllerNum) : 
BBBGameController(model, display), controllerNum(controllerNum), vibrateLengthInSeconds(0.0), vibrateTimeTracker(0.0),
directionMagnitudePercent(0.0f) {
	this->enterActionOn = this->leftActionOn = this->rightActionOn =
	this->upActionOn = this->downActionOn =	this->escapeActionOn = 
    this->pauseActionOn = this->specialDirOn = this->triggerActionOn = false;
}

XBox360Controller::~XBox360Controller() {
}

/**
 * Cause the XBox controller to vibrate its two motors...
 */
void XBox360Controller::Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) {
	this->SetVibration(leftMotorAmt, rightMotorAmt);
	this->vibrateLengthInSeconds = lengthInSeconds;
	this->vibrateTimeTracker     = 0.0;
}

void XBox360Controller::ClearVibration() {
    this->SetVibration(BBBGameController::NoVibration, BBBGameController::NoVibration);
    this->vibrateTimeTracker = this->vibrateLengthInSeconds = 0.0;
}

void XBox360Controller::SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) {
	// Create a Vibraton State
	XINPUT_VIBRATION vibration;

	// Clear the Vibration state
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	vibration.wLeftMotorSpeed  = GetXBoxVibrationAmtFromEnum(leftMotorAmt);
	vibration.wRightMotorSpeed = GetXBoxVibrationAmtFromEnum(rightMotorAmt);

	// Vibrate the controller
	XInputSetState(this->controllerNum, &vibration);
}

void XBox360Controller::NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState) {
	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		if (!this->enterActionOn) {
			this->display->ButtonPressed(GameControl::EnterButtonAction);
			this->enterActionOn = true;
		}
	}
	else {
		if (this->enterActionOn) {
			this->display->ButtonReleased(GameControl::EnterButtonAction);
			this->enterActionOn = false;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B || controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
		if (!this->escapeActionOn) {
			this->display->ButtonPressed(GameControl::EscapeButtonAction);
			this->escapeActionOn = true;
		}
	}
	else {
		if (this->escapeActionOn) {
			this->display->ButtonReleased(GameControl::EscapeButtonAction);
			this->escapeActionOn = false;
		}
	}


    this->UpdateDirections(controllerState, 2*XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

void XBox360Controller::InGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState) {
    if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
        //controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B ||
        //controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X ||
        //controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {

		if (!this->enterActionOn) {
            this->model->ShootActionReleaseUse();
			this->display->ButtonPressed(GameControl::EnterButtonAction);
			this->enterActionOn = true;
		}
	}
	else {
		if (this->enterActionOn) {
			this->display->ButtonReleased(GameControl::EnterButtonAction);
			this->enterActionOn = false;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
		if (!this->escapeActionOn) {
			this->display->ButtonPressed(GameControl::EscapeButtonAction);
			this->escapeActionOn = true;
		}
	}
	else {
		if (this->escapeActionOn) {
			this->display->ButtonReleased(GameControl::EscapeButtonAction);
			this->escapeActionOn = false;
		}
	}

    //float addedSensitivityDeadZoneLThumb = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * (1.0f - XBox360Controller::GetSensitivityFraction());
    //float addedSensitivityDeadZoneRThumb = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * (1.0f - XBox360Controller::GetSensitivityFraction());

    this->UpdateDirections(controllerState, static_cast<int>(1.85f * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));

	// Triggers
	if (controllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD || 
        controllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {

        if (!this->triggerActionOn) {
		    this->model->ShootActionReleaseUse();
            this->triggerActionOn = true;
        }
    }
    else if (this->triggerActionOn) {
        this->triggerActionOn = false;
    }

    // Special stuff (ball bullet time)...
    if (abs(controllerState.Gamepad.sThumbRX) > (XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ||
        abs(controllerState.Gamepad.sThumbRY) > (XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {


        this->display->SpecialDirectionPressed(controllerState.Gamepad.sThumbRX, 
                                               controllerState.Gamepad.sThumbRY);
        this->specialDirOn = true;

    }
    else {
        if (this->specialDirOn) {
            this->display->SpecialDirectionReleased();
            this->specialDirOn = false;
        }
    }
}

bool XBox360Controller::ProcessState() {
	// Clear the state
	XINPUT_STATE controllerState;
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	// Get the current state of the controller
	XInputGetState(this->controllerNum, &controllerState);

	// Using the current state determine what actions to take in the game...

	// Basic game pad buttons - functionality depends on whether we're in-game or not...
	if (this->display->GetCurrentDisplayState() == DisplayState::InGame ||
        this->display->GetCurrentDisplayState() == DisplayState::InGameBossLevel ||
        this->display->GetCurrentDisplayState() == DisplayState::InTutorialGame) {
		this->InGameOnProcessStateSpecificActions(controllerState);
	}
	else {
		this->NotInGameOnProcessStateSpecificActions(controllerState);
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
		if (!this->pauseActionOn) {
			this->display->ButtonPressed(GameControl::PauseButtonAction);
			this->pauseActionOn = true;
		}
	}
	else {
		if (this->pauseActionOn) {
			this->display->ButtonReleased(GameControl::PauseButtonAction);
			this->pauseActionOn = false;
		}
	}

	return false;
}

void XBox360Controller::UpdateDirections(const XINPUT_STATE& controllerState, 
                                         int sensitivityLeft) {
	// Movement controls:
	// D-Pad
	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ||
		(abs(controllerState.Gamepad.sThumbLY) > sensitivityLeft && controllerState.Gamepad.sThumbLY > 0)) {

		if (!this->upActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLY);
            }

			this->display->ButtonPressed(GameControl::UpButtonAction, magnitude);
			this->upActionOn = true;
		}
	}
	else {
		if (this->upActionOn) {
			this->display->ButtonReleased(GameControl::UpButtonAction);
			this->upActionOn = false;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
		(abs(controllerState.Gamepad.sThumbLY) > sensitivityLeft && controllerState.Gamepad.sThumbLY < 0)) {

		if (!this->downActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLY);
            }

			this->display->ButtonPressed(GameControl::DownButtonAction, magnitude);
			this->downActionOn = true;
		}
	}
	else {
		if (this->downActionOn) {
			this->display->ButtonReleased(GameControl::DownButtonAction);
			this->downActionOn = false;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || 
		  (abs(controllerState.Gamepad.sThumbLX) > sensitivityLeft && controllerState.Gamepad.sThumbLX < 0)) {

		if (!this->leftActionOn && !this->rightActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLX);
            }

			this->display->ButtonPressed(GameControl::LeftButtonAction, magnitude);
			this->leftActionOn = true;
		}

        if (this->leftActionOn) {
            this->directionMagnitudePercent = static_cast<float>(abs(controllerState.Gamepad.sThumbLX) - sensitivityLeft) /
                static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
        }

	}
	else {
		if (this->leftActionOn) {
			this->display->ButtonReleased(GameControl::LeftButtonAction);
			this->leftActionOn = false;
            this->directionMagnitudePercent = 0.0f;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || 
		  (abs(controllerState.Gamepad.sThumbLX) > sensitivityLeft && controllerState.Gamepad.sThumbLX > 0)) {

		if (!this->rightActionOn && !this->leftActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLX);
            }

			this->display->ButtonPressed(GameControl::RightButtonAction, magnitude);
			this->rightActionOn = true;
		}

        if (this->rightActionOn) {
            this->directionMagnitudePercent = static_cast<float>(abs(controllerState.Gamepad.sThumbLX) - sensitivityLeft) /
                static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
        }
	}
	else {
		if (this->rightActionOn) {
			this->display->ButtonReleased(GameControl::RightButtonAction);
			this->rightActionOn = false;
            this->directionMagnitudePercent = 0.0f;
		}
	}

}

void XBox360Controller::Sync(size_t frameID, double dT) {
	UNUSED_PARAMETER(dT);

	// Paddle controls (NOTE: the else is to make the feedback more exact)
	if (this->leftActionOn) {
		PlayerPaddle::PaddleMovement leftDir = this->model->AreControlsFlipped() ? PlayerPaddle::RightPaddleMovement : PlayerPaddle::LeftPaddleMovement;
		this->model->MovePaddle(frameID, leftDir, this->directionMagnitudePercent);
        //debug_output("Left move magnitude: " << this->directionMagnitudePercent);
	}
	else if (this->rightActionOn) {
		PlayerPaddle::PaddleMovement rightDir = this->model->AreControlsFlipped() ? PlayerPaddle::LeftPaddleMovement : PlayerPaddle::RightPaddleMovement;
		this->model->MovePaddle(frameID, rightDir, this->directionMagnitudePercent);
        //debug_output("Right move magnitude: " << this->directionMagnitudePercent);
	}
	else {
		this->model->MovePaddle(frameID, PlayerPaddle::NoPaddleMovement, 0.0);
	}

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


	// Execute any debug functionality for when a button is held down...
	//this->DebugRepeatActions();
}

bool XBox360Controller::IsConnected() const {
	return XBox360Controller::IsConnected(this->controllerNum);
}

bool XBox360Controller::IsConnected(int controllerNum) {
	// Clear the state...
	XINPUT_STATE controllerState;
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));

	// Get the state of the controller - this will tell us whether it's even connected to
	// begin with...
	DWORD Result = XInputGetState(controllerNum, &controllerState);
	if (Result == ERROR_SUCCESS) {
		return true;
	}

	return false;
}

void XBox360Controller::DebugRepeatActions() {

}

GameControl::ActionMagnitude XBox360Controller::GetMagnitudeForThumbpad(int16_t value) {

    const int SMALL_MAGNITUDE_MAX_VALUE  = 10000;
    const int NORMAL_MAGNITUDE_MAX_VALUE = SMALL_MAGNITUDE_MAX_VALUE  + 10000;
    const int LARGE_MAGNITUDE_MAX_VALUE  = NORMAL_MAGNITUDE_MAX_VALUE + 12000;
    
    int absValue = abs(value);
    
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

#else // _WIN32 is not defined...

XBox360Controller::XBox360Controller(GameModel* model, GameDisplay* display, int controllerNum) : 
BBBGameController(model, display) {
}

XBox360Controller::~XBox360Controller() {
}

void XBox360Controller::Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) {
	UNUSED_PARAMETER(lengthInSeconds);
	UNUSED_PARAMETER(leftMotorAmt);
	UNUSED_PARAMETER(rightMotorAmt);
}

void XBox360Controller::ProcessState() {
}

void XBox360Controller::Sync(double dT) {
	UNUSED_PARAMETER(dT);
}

bool XBox360Controller::IsConnected() const {
	return false;
}

bool XBox360Controller::IsConnected(int controllerNum) {
	UNUSED_PARAMETER(controllerNum);
	return false;
}

void XBox360Controller::DebugRepeatActions() {
}

void XBox360Controller::NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState) {
	UNUSED_PARAMETER(controllerState);
}
void XBox360Controller::GameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState) {
	UNUSED_PARAMETER(controllerState);
}
void XBox360Controller::SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) {
	UNUSED_PARAMETER(leftMotorAmt);
	UNUSED_PARAMETER(rightMotorAmt);
}

GameControl::ActionMagnitude XBox360Controller::GetMagnitudeForThumbpad(int16_t value) {
    UNUSED_PARAMETER(value);
}
#endif