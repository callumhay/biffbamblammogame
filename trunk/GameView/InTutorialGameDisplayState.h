/**
 * InTutorialGameDisplayState.h
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

#ifndef __INTUTORIALGAMEDISPLAYSTATE_H__
#define __INTUTORIALGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/TextLabel.h"
#include "../ESPEngine/ESP.h"
#include "../GameModel/GameModel.h"

#include "DisplayState.h"
#include "TutorialInGameRenderPipeline.h"
#include "CountdownHUD.h"
#include "GameDisplay.h"
#include "MouseRenderer.h"
#include "CgFxPostTutorialAttention.h"
#include "TutorialHintListeners.h"

class TutorialHint;
class TutorialEventsListener;

/**
 * This class is the game state where the game is in play but the current
 * level is the tutorial level and therefore has various graphical hints
 * and other ease-of-play characteristics activated.
 */
class InTutorialGameDisplayState : public DisplayState {
public:
	InTutorialGameDisplayState(GameDisplay* display);
	~InTutorialGameDisplayState();

    bool AllowsGameModelUpdates() const { return true; }
	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);
    void MouseMoved(int oglWindowCoordX, int oglWindowCoordY);
    void WindowFocus(bool windowHasFocus);
	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    GameModel::Difficulty beforeTutorialDifficulty;

	TutorialInGameRenderPipeline renderPipeline;
    TutorialEventsListener* tutorialListener;

    typedef std::vector<EmbededTutorialHint*> HintList;
    typedef HintList::iterator HintListIter;

    HintList noDepthTutorialHints;
    HintList ballFollowTutorialHints;

    CountdownHUD boostCountdownHUD;
    CgFxPostTutorialAttention tutorialAttentionEffect;

    void InitTutorialHints();

    class HintListener : public TutorialHintListener  {
    public:
        HintListener(InTutorialGameDisplayState* state) : TutorialHintListener(), state(state) {
            assert(state != NULL);
        }
        virtual ~HintListener() {}

        void OnTutorialHintShown();
        void OnTutorialHintUnshown();
    protected:
        InTutorialGameDisplayState* state;
    };

    class MovePaddleHintListener : public HintListener {
    public:
        MovePaddleHintListener(InTutorialGameDisplayState* state) : HintListener(state) {}
        ~MovePaddleHintListener() {
            PlayerPaddle::SetEnablePaddleRelease(true);
        }

        void OnTutorialHintShown();
        void OnTutorialHintUnshown();
    };

    class SlowBallHintListener : public HintListener {
    public:
        SlowBallHintListener(InTutorialGameDisplayState* state) : HintListener(state) {}
        ~SlowBallHintListener() {}

        void OnTutorialHintShown();
        void OnTutorialHintUnshown();

    private:
        void SetBallSpeed(float speed);
    };

    DISALLOW_COPY_AND_ASSIGN(InTutorialGameDisplayState);
};

inline void InTutorialGameDisplayState::MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
    this->display->GetMouse()->ShowMouse(oglWindowCoordX, oglWindowCoordY);
}

inline void InTutorialGameDisplayState::WindowFocus(bool windowHasFocus) {
    this->display->GetMouse()->SetWindowHasFocus(windowHasFocus);
}

inline DisplayState::DisplayStateType InTutorialGameDisplayState::GetType() const {
    return DisplayState::InTutorialGame;
}

#endif // __INTUTORIALGAMEDISPLAYSTATE_H__