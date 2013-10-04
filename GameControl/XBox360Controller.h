/**
 * XBox360Controller.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
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

	bool ProcessState();
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

	double vibrateTimeTracker;
	double vibrateLengthInSeconds;

    float directionMagnitudePercentLeftRight;
    float directionMagnitudePercentUpDown;

	void NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);
	void InGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);

	void SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void UpdateDirections(const XINPUT_STATE& controllerState, int sensitivityLeft);

	void DebugRepeatActions();

    static float GetSensitivityFraction();
    static GameControl::ActionMagnitude GetMagnitudeForThumbpad(int16_t value);

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