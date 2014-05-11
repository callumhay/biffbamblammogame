/**
 * KeyboardSDLController.h
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

	bool ProcessState(double dT);
	void Sync(size_t frameID, double dT);

private:
    static const double TIME_TO_MAX_SPEED;

    double dirHeldDownTimeCounter;
    bool windowHasFocus;

    //int lastPaddleLeftDir;
    //int lastOtherLeftDir;
    //int lastPaddleRightDir;
    //int lastOtherRightDir;

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

    void WindowActiveEvent(const SDL_ActiveEvent& sdlActiveEvent);

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