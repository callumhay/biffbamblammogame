/**
 * XBox360Controller.h
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

#ifndef __XBOX360GAMECONTROLLER_H__
#define __XBOX360GAMECONTROLLER_H__

#include "BBBGameController.h"
#include "GameControl.h"

#ifdef _WIN32
#include <windows.h>
#include <XInput.h>
#else
class XINPUT_STATE;
#endif


class XBox360Controller : public BBBGameController {
public:
	static const int MAX_CONTROLLERS = 4;

    static const int MIN_SENSITIVITY     = 1;
    static const int MAX_SENSITIVITY     = 100;
    static const int DEFAULT_SENSITIVITY = 100;

	XBox360Controller(GameModel* model, GameDisplay* display, int controllerNum);
	~XBox360Controller();

	static bool IsConnected(int controllerNum);
    static bool SetSensitivity(int sensitivity);

	void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void ClearVibration();

	bool ProcessState(double dT);
	void Sync(size_t frameID, double dT);
	bool IsConnected() const;

private:
    //static const double TIME_UNTIL_PADDLE_MOVE_SYNC_RESET_IN_S;
    static int sensitivity;

	int controllerNum;

	bool enterActionOn;
	bool leftActionOn;
	bool rightActionOn;
	bool upActionOn;
	bool downActionOn;
	bool escapeActionOn;
	bool pauseActionOn;
    bool specialDirOn;
    bool triggerActionOn;
    bool leftBumperActionOn;
    bool rightBumperActionOn;

	double vibrateTimeTracker;
	double vibrateLengthInSeconds;

    float directionMagnitudePercentLeftRight;
    float directionMagnitudePercentUpDown;

	void NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);
	void InGameOnProcessStateSpecificActions(double dT, const XINPUT_STATE& controllerState);

	void SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void UpdateDirections(const XINPUT_STATE& controllerState, int sensitivityLeft);

	void DebugRepeatActions();

    static float GetSensitivityFraction();
    static GameControl::ActionMagnitude GetMagnitudeForThumbpad(int16_t v);

	DISALLOW_COPY_AND_ASSIGN(XBox360Controller);
};

inline bool XBox360Controller::SetSensitivity(int sensitivity) {
    if (sensitivity > MAX_SENSITIVITY || sensitivity < MIN_SENSITIVITY) {
        return false;
    }
    XBox360Controller::sensitivity = sensitivity;
    return true;
}

inline float XBox360Controller::GetSensitivityFraction() {
    return static_cast<float>(XBox360Controller::sensitivity) / static_cast<float>(XBox360Controller::MAX_SENSITIVITY);
}

#endif // __XBOX360GAMECONTROLLER_H__