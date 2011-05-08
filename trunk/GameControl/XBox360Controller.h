/**
 * XBox360Controller.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __XBOX360GAMECONTROLLER_H__
#define __XBOX360GAMECONTROLLER_H__

#include "BBBGameController.h"

#ifdef _WIN32
#include <windows.h>
#include <XInput.h>
#else
class XINPUT_STATE;
#endif


class XBox360Controller : public BBBGameController {
public:
	static const int MAX_CONTROLLERS = 4;

	XBox360Controller(GameModel* model, GameDisplay* display, int controllerNum);
	~XBox360Controller();

	static bool IsConnected(int controllerNum);

	void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void ClearVibration();

	bool ProcessState();
	void Sync(size_t frameID, double dT);
	bool IsConnected() const;

private:
	int controllerNum;

	bool enterActionOn;
	bool leftActionOn;
	bool rightActionOn;
	bool upActionOn;
	bool downActionOn;
	bool escapeActionOn;
	bool pauseActionOn;
    bool specialDirOn;

	double vibrateTimeTracker;
	double vibrateLengthInSeconds;

	void NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);
	void InGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);

	void SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void UpdateDirections(const XINPUT_STATE& controllerState, int sensitivityLeft);


	void DebugRepeatActions();

	DISALLOW_COPY_AND_ASSIGN(XBox360Controller);
};

#endif // __XBOX360GAMECONTROLLER_H__