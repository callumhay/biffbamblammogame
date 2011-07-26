/**
 * KeyboardSDLController.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __KEYBOARDSDLCONTROLLER_H__
#define __KEYBOARDSDLCONTROLLER_H__

#include "BBBGameController.h"
#include "../BlammoEngine/Vector.h"

// SDL library include
#include "SDL.h"

class KeyboardSDLController : public BBBGameController {
public:
	KeyboardSDLController(GameModel* model, GameDisplay* display);
	~KeyboardSDLController();

	void Vibrate(double lengthInSeconds, const VibrateAmount& leftMotorAmt, const VibrateAmount& rightMotorAmt);
    void ClearVibration();
	bool IsConnected() const;

	bool ProcessState();
	void Sync(size_t frameID, double dT);

private:
    bool specialDirOn;

	bool keyPressed[SDLK_LAST];
	void SetKeyPress(int key, bool isPressed);

	void KeyDown(SDLKey key);
	void KeyUp(SDLKey key);

    void MouseButtonDown(unsigned int button, unsigned int x, unsigned int y);
    void MouseButtonUp(unsigned int button);
    void MouseMotion(unsigned int x, unsigned int y, int relX, int relY);

	void ExecuteDisplayKeyPressedNotifications(SDLKey key);
	void ExecuteDisplayKeyReleasedNotifications(SDLKey key);

    

    void GetSpecialDirectionVector(Vector2D& dir);

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

inline void KeyboardSDLController::ClearVibration() {
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

inline void KeyboardSDLController::GetSpecialDirectionVector(Vector2D& dir) {
    if (this->keyPressed[SDLK_w]) {
        dir[0] += 0;
        dir[1] += 1;
    }
    if (this->keyPressed[SDLK_a]) {
        dir[0] += -1;
        dir[1] += 0;
    }
    if (this->keyPressed[SDLK_s]) {
        dir[0] += 0;
        dir[1] += -1;
    }
    if (this->keyPressed[SDLK_d]) {
        dir[0] += 1;
        dir[1] += 0;
    }
}

#endif // __KEYBOARDSDLCONTROLLER_H__