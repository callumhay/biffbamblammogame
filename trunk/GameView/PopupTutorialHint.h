/**
 * PopupTutorialHint.h
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

#ifndef __POPUPTUTORIALHINT_H__
#define __POPUPTUTORIALHINT_H__

#include "TutorialHint.h"
#include "DecoratorOverlayPane.h"

#include "../BlammoEngine/Camera.h"

class PopupTutorialHint;

class PaneHandler : public OverlayPaneEventHandler {
public:
    PaneHandler(PopupTutorialHint* popupHint) : popupHint(popupHint) {
    }
    virtual ~PaneHandler() {};

    void SetPopupTutorialHint(PopupTutorialHint* popup) { this->popupHint = popup; }

    virtual void OptionSelected(const std::string& optionText);

protected:
    PopupTutorialHint* popupHint;
};

class PopupTutorialHint : public TutorialHint {
public:
    PopupTutorialHint(size_t width);
    PopupTutorialHint(size_t width, PaneHandler* handler);
    ~PopupTutorialHint();

    DecoratorOverlayPane* GetPane() const {
        return this->pane;
    }

    void ButtonPressed(const GameControl::ActionButton& pressedButton);
    //void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    //void MousePressed(const GameControl::MouseButton& pressedButton)     { UNUSED_PARAMETER(pressedButton);  }
    //void MouseReleased(const GameControl::MouseButton& releasedButton)   { UNUSED_PARAMETER(releasedButton); }

    void Show(double delayInSeconds, double fadeInTimeInSeconds);
    void Unshow(double delayInSeconds, double fadeOutTimeInSeconds, bool overridePrevUnshow = false);

    void Tick(double dT);
    void Draw(const Camera& camera, bool drawWithDepth = false, float depth = 0.0f);

private:
    static const double UNSHOW_TIME;

    DecoratorOverlayPane* pane;
    OverlayPaneEventHandler* paneHandler;

    double timeToUnshow;
    DISALLOW_COPY_AND_ASSIGN(PopupTutorialHint);
};

inline void PopupTutorialHint::Draw(const Camera& camera, bool, float) {
    UNUSED_PARAMETER(camera);
    this->pane->Draw(Camera::GetWindowWidth(), Camera::GetWindowHeight());
}

#endif // __POPUPTUTORIALHINT_H__