/**
 * TutorialHint.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __TUTORIALHINT_H__
#define __TUTORIALHINT_H__

#include "../BlammoEngine/Animation.h"
#include "../GameControl/GameControl.h"

class GameModel;
class TutorialHintListener;
class Camera;

/**
 * Abstract super-class for all tutorial hints - graphical hints that pop-up
 * during the tutorial level of the game to guide and teach the user.
 */
class TutorialHint {
public:
    TutorialHint();
    virtual ~TutorialHint();

    void SetListener(TutorialHintListener* listener);
    bool GetIsVisible() const { return this->isShown; }

    // Triggers - these can do various things to the tooltip depending on how they are
    // overriden in child classes
    virtual void ButtonPressed(const GameControl::ActionButton& pressedButton)   { UNUSED_PARAMETER(pressedButton);  }
    virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    virtual void MousePressed(const GameControl::MouseButton& pressedButton)     { UNUSED_PARAMETER(pressedButton);  }
    virtual void MouseReleased(const GameControl::MouseButton& releasedButton)   { UNUSED_PARAMETER(releasedButton); }

    virtual void Show(double delayInSeconds, double fadeInTimeInSeconds) = 0;
    virtual void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false) = 0;

    virtual void Tick(double dT) = 0;
    virtual void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f) = 0;

protected:
    static const float FADE_IN_SCALE_START;
    static const float FADE_OUT_SCALE_END;

    TutorialHintListener* listener;
    bool isShown;

private:
    DISALLOW_COPY_AND_ASSIGN(TutorialHint);
};

#endif // __TUTORIALHINT_H__