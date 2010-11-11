#ifndef __KEYBOARDSDLCONTROLLER_H__
#define __KEYBOARDSDLCONTROLLER_H__

#include "BBBGameController.h"

// SDL library include
#include "SDL.h"

class KeyboardSDLController : public BBBGameController {
public:
	KeyboardSDLController(GameModel* model, GameDisplay* display);
	~KeyboardSDLController();

	void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
	bool IsConnected() const;

	bool ProcessState();
	void Sync(size_t frameID, double dT);

private:
	bool keyPressed[SDLK_LAST];
	void SetKeyPress(int key, bool isPressed);

	void KeyDown(SDLKey key);
	void KeyUp(SDLKey key);

	void ExecuteDisplayKeyPressedNotifications(SDLKey key);
	void ExecuteDisplayKeyReleasedNotifications(SDLKey key);
	
	void DebugRepeatActions();
	void DebugKeyDownActions(SDLKey key);

	DISALLOW_COPY_AND_ASSIGN(KeyboardSDLController);
};

inline void KeyboardSDLController::Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt) {
	UNUSED_PARAMETER(lengthInSeconds);
	UNUSED_PARAMETER(leftMotorAmt);
	UNUSED_PARAMETER(rightMotorAmt);
	// Unfortunately, keyboards tend not to vibrate.
}

inline bool KeyboardSDLController::IsConnected() const {
	return true;
}

// Helper function for setting values in the keyPressed array.
inline void KeyboardSDLController::SetKeyPress(int key, bool isPressed) {
	if (key < 0 || key >= SDLK_LAST) { 
		return; 
	}
	this->keyPressed[key] = isPressed;
	//std::cout << "KeyPressed (" << isPressed << ") : " << SDL_GetKeyName((SDLKey)key) << std::endl;
}

#endif // __KEYBOARDSDLCONTROLLER_H__