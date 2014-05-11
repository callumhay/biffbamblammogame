/**
 * GameControl.h
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

#ifndef __GAMECONTROL_H__
#define __GAMECONTROL_H__

class GameControl {
public:
    enum ActionButton { UpButtonAction, DownButtonAction, LeftButtonAction, RightButtonAction, 
                        EnterButtonAction, EscapeButtonAction, PauseButtonAction, SpecialCheatButtonAction,
                        OtherButtonAction, LeftBumperAction, RightBumperAction };

    enum ActionMagnitude { ZeroMagnitude, SmallMagnitude, NormalMagnitude, LargeMagnitude, FullMagnitude };

    enum MouseButton { LeftMouseButton, RightMouseButton };

    static bool IsDirectionActionButton(const ActionButton& button);

private:
	GameControl(){};
	~GameControl(){};
};

inline bool GameControl::IsDirectionActionButton(const ActionButton& button) {
    return (button == UpButtonAction || button == DownButtonAction || button == LeftButtonAction || button == RightButtonAction);
}

#endif // __GAMECONTROLACTIONBUTTONS_H__