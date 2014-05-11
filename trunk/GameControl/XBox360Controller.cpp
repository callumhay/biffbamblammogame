/**
 * XBox360Controller.cpp
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

#include "XBox360Controller.h"

#ifdef _WIN32

#include "GameControl.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameDisplay.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

//const double XBox360Controller::TIME_UNTIL_PADDLE_MOVE_SYNC_RESET_IN_S = 1.25;

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
directionMagnitudePercentLeftRight(0.0f), directionMagnitudePercentUpDown(0.0) {

	this->enterActionOn = this->leftActionOn = this->rightActionOn =
	this->upActionOn = this->downActionOn =	this->escapeActionOn = 
    this->pauseActionOn = this->specialDirOn = this->triggerActionOn = 
    this->leftBumperActionOn = this->rightBumperActionOn = false;
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

void XBox360Controller::InGameOnProcessStateSpecificActions(double dT, const XINPUT_STATE& controllerState) {
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

    this->UpdateDirections(controllerState, static_cast<int>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));

	// Triggers
	if (controllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD || 
        controllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {

        if (!this->triggerActionOn) {
            this->model->ShootActionReleaseUse();
            this->triggerActionOn = true;
        }

        this->model->ShootActionContinuousUse(dT,
            (std::max<float>(controllerState.Gamepad.bLeftTrigger, controllerState.Gamepad.bRightTrigger) - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / 
            static_cast<float>(std::numeric_limits<BYTE>::max() - XINPUT_GAMEPAD_TRIGGER_THRESHOLD));

    }
    else if (this->triggerActionOn) {
        this->triggerActionOn = false;
        this->model->ShootActionContinuousUse(dT, 0.0f);
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

bool XBox360Controller::ProcessState(double dT) {

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
		this->InGameOnProcessStateSpecificActions(dT, controllerState);
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

    if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
        if (!this->leftBumperActionOn) {
            this->display->ButtonPressed(GameControl::LeftBumperAction);
            this->leftBumperActionOn = true;
        }
    }
    else {
        if (this->leftBumperActionOn) {
            this->display->ButtonReleased(GameControl::LeftBumperAction);
            this->leftBumperActionOn = false;
        }
    }

    if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
        if (!this->rightBumperActionOn) {
            this->display->ButtonPressed(GameControl::RightBumperAction);
            this->rightBumperActionOn = true;
        }
    }
    else {
        if (this->rightBumperActionOn) {
            this->display->ButtonReleased(GameControl::RightBumperAction);
            this->rightBumperActionOn = false;
        }
    }

	return false;
}

void XBox360Controller::UpdateDirections(const XINPUT_STATE& controllerState, 
                                         int sensitivityLeft) {
	// Movement controls:
	// D-Pad
	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ||
		(abs(controllerState.Gamepad.sThumbLY) >= sensitivityLeft && controllerState.Gamepad.sThumbLY > 0)) {

		if (!this->upActionOn && !this->downActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLY);
            }

            // Only do the button press if the magnitude is greater in the y-axis than the x
            if (!(controllerState.Gamepad.wButtons & (XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT)) && 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLY)) >= 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLX))) {

			    this->display->ButtonPressed(GameControl::UpButtonAction, magnitude);
            }
			this->upActionOn = true;
		}

        if (this->upActionOn) {
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
                this->directionMagnitudePercentUpDown = 1.0f;
            }
            else {
                this->directionMagnitudePercentUpDown = static_cast<float>(abs(controllerState.Gamepad.sThumbLY) - sensitivityLeft) /
                    static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
            }
        }
	}
	else {
		if (this->upActionOn) {
			this->display->ButtonReleased(GameControl::UpButtonAction);
			this->upActionOn = false;
            this->directionMagnitudePercentUpDown = 0.0f;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
		(abs(controllerState.Gamepad.sThumbLY) >= sensitivityLeft && controllerState.Gamepad.sThumbLY < 0)) {

		if (!this->downActionOn && !this->upActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLY);
            }

            // Only do the button press if the magnitude is greater in the y-axis than the x
            if (!(controllerState.Gamepad.wButtons & (XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT)) && 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLY)) >= 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLX))) {

			    this->display->ButtonPressed(GameControl::DownButtonAction, magnitude);
            }

			this->downActionOn = true;
		}

        if (this->downActionOn) {
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
                this->directionMagnitudePercentUpDown = 1.0f;
            }
            else {
                this->directionMagnitudePercentUpDown = static_cast<float>(abs(controllerState.Gamepad.sThumbLY) - sensitivityLeft) /
                    static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
            }
        }
	}
	else {
		if (this->downActionOn) {
			this->display->ButtonReleased(GameControl::DownButtonAction);
			this->downActionOn = false;
            this->directionMagnitudePercentUpDown = 0.0f;
		}
	}


	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || 
		  (abs(controllerState.Gamepad.sThumbLX) >= sensitivityLeft && controllerState.Gamepad.sThumbLX < 0)) {

		if (!this->leftActionOn && !this->rightActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                magnitude = GameControl::FullMagnitude;

            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLX);
            }

            // We do a button pressed only if the magnitude is greater on the x-axis than the y
            if (!(controllerState.Gamepad.wButtons & (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN)) && 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLX)) >=
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLY))) {

			    this->display->ButtonPressed(GameControl::LeftButtonAction, magnitude);
            }

			this->leftActionOn = true;
		}

        if (this->leftActionOn) {
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                this->directionMagnitudePercentLeftRight = 1.0f;
            }
            else {
                this->directionMagnitudePercentLeftRight = static_cast<float>(abs(controllerState.Gamepad.sThumbLX) - sensitivityLeft) /
                    static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
            }
        }

	}
	else {
		if (this->leftActionOn) {
			this->display->ButtonReleased(GameControl::LeftButtonAction);
			this->leftActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
		}
	}

	if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || 
		  (abs(controllerState.Gamepad.sThumbLX) >= sensitivityLeft && controllerState.Gamepad.sThumbLX > 0)) {

		if (!this->rightActionOn && !this->leftActionOn) {

            GameControl::ActionMagnitude magnitude;
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                magnitude = GameControl::FullMagnitude;
            }
            else {
                magnitude = XBox360Controller::GetMagnitudeForThumbpad(controllerState.Gamepad.sThumbLX);
            }

            // We do a button pressed only if the magnitude is greater on the x-axis than the y
			if (!(controllerState.Gamepad.wButtons & (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN)) && 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLX)) >= 
                std::max<SHORT>(sensitivityLeft, abs(controllerState.Gamepad.sThumbLY))) {

                this->display->ButtonPressed(GameControl::RightButtonAction, magnitude);
            }

			this->rightActionOn = true;
		}

        if (this->rightActionOn) {
            if (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                this->directionMagnitudePercentLeftRight = 1.0f;
            }
            else {
                this->directionMagnitudePercentLeftRight = static_cast<float>(abs(controllerState.Gamepad.sThumbLX) - sensitivityLeft) /
                    static_cast<float>(std::numeric_limits<int16_t>::max() - sensitivityLeft);
            }
        }
	}
	else {
		if (this->rightActionOn) {
			this->display->ButtonReleased(GameControl::RightButtonAction);
			this->rightActionOn = false;
            this->directionMagnitudePercentLeftRight = 0.0f;
		}
	}

}

void XBox360Controller::Sync(size_t frameID, double dT) {
    
	// Paddle controls (NOTE: the else is to make the feedback more exact)
	if (this->leftActionOn) {

		int leftDir = this->model->AreControlsFlippedForPaddle() ? 1 : -1;
        this->model->MovePaddle(frameID, leftDir, this->directionMagnitudePercentLeftRight);
        leftDir = this->model->AreControlsFlippedForOther() ? 1 : -1;
        this->model->MoveOther(frameID, leftDir, this->directionMagnitudePercentLeftRight);
	}
	else if (this->rightActionOn) {

		int rightDir = this->model->AreControlsFlippedForPaddle() ? -1 : 1;
        this->model->MovePaddle(frameID, rightDir, this->directionMagnitudePercentLeftRight);
        rightDir = this->model->AreControlsFlippedForOther() ? -1 : 1;
        this->model->MoveOther(frameID, rightDir, this->directionMagnitudePercentLeftRight);
	}
	else {
		this->model->MovePaddle(frameID, 0, 0.0);
        this->model->MoveOther(frameID, 0, 0.0);
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

GameControl::ActionMagnitude XBox360Controller::GetMagnitudeForThumbpad(int16_t v) {

    const int SMALL_MAGNITUDE_MAX_VALUE  = 10000;
    const int NORMAL_MAGNITUDE_MAX_VALUE = SMALL_MAGNITUDE_MAX_VALUE  + 8000;
    const int LARGE_MAGNITUDE_MAX_VALUE  = NORMAL_MAGNITUDE_MAX_VALUE + 7000;
    
    int absValue = abs(v);
    
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

void XBox360Controller::ProcessState(double dT) {
	UNUSED_PARAMETER(dT);
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