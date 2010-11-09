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
	bool ProcessState();
	void Sync(double dT);
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


	double vibrateTimeTracker;
	double vibrateLengthInSeconds;

	void NotInGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);
	void InGameOnProcessStateSpecificActions(const XINPUT_STATE& controllerState);

	void SetVibration(const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);

	void DebugRepeatActions();

	DISALLOW_COPY_AND_ASSIGN(XBox360Controller);
};

#endif // __XBOX360GAMECONTROLLER_H__