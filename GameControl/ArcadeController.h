/**
 * ArcadeController.h
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

#ifndef __ARCADECONTROLLER_H__
#define __ARCADECONTROLLER_H__

#include "KeyboardSDLController.h"
#include "GameControl.h"

class ArcadeControllerEventsListener;
class ArcadeSerialComm;

class ArcadeController : public KeyboardSDLController {
public:
    ArcadeController(GameModel* model, GameDisplay* display);
    ~ArcadeController();

    static bool IsConnected(int joystickIdx);
    bool IsConnected() const;

    // ****NOTE: IF YOU IMPLEMENT VIBRATION:
    // DONT FORGET TO FIX TIMER CODE IN SYNC METHOD AS WELL!!

    bool ProcessState(double dT);
    void Sync(size_t frameID, double dT);

    void SetSerialPort(const std::string& serialPort);

private:

    ArcadeControllerEventsListener* eventsListener;
    ArcadeSerialComm* serialComm;

    std::vector<SDL_Joystick*> joysticks;
    Sint16 joystickX, joystickY;

    bool leftActionOn;
    bool rightActionOn;
    bool upActionOn;
    bool downActionOn;
    bool specialDirOn;

    SDLKey fireKey;
    SDLKey boostKey;

    float directionMagnitudePercentLeftRight;
    float directionMagnitudePercentUpDown;

    void JoystickMotion(const SDL_JoyAxisEvent& joyAxisEvent);
    void KeyDown(SDLKey key);
    void KeyUp(SDLKey key);

    bool isFireActionOn() const;
    bool isBoostActionOn() const;

    static GameControl::ActionMagnitude GetMagnitudeForJoystick(Sint16 m);

	DISALLOW_COPY_AND_ASSIGN(ArcadeController);
};

inline bool ArcadeController::IsConnected(int joystickIdx) {
    return SDL_JoystickOpened(joystickIdx) == 1;
}

inline bool ArcadeController::IsConnected() const {
    bool anyJoystickConnected = false;
    for (int i = 0; i < static_cast<int>(this->joysticks.size()); i++) {
        if (ArcadeController::IsConnected(SDL_JoystickIndex(this->joysticks[i]))) {
            anyJoystickConnected = true;
            break;
        }
    }

    return !this->joysticks.empty() && anyJoystickConnected;
}

inline bool ArcadeController::isFireActionOn() const {
    return this->keyPressed[this->fireKey];
}

inline bool ArcadeController::isBoostActionOn() const {
    return this->keyPressed[this->boostKey];
}

#endif // __ARCADECONTROLLER_H__