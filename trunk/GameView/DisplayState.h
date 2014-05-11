/**
 * DisplayState.h
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

#ifndef __DISPLAYSTATE_H__
#define __DISPLAYSTATE_H__

#include "GameViewConstants.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameControl/GameControl.h"

class GameDisplay;
class GameModel;
class CgFxSkybox;
class Camera;

class DisplayStateInfo {
public:
    DisplayStateInfo() : worldSelectionIdx(-1), worldUnlockIdx(-1), levelSelectionIdx(-1), doAnimatedFadeIn(false) {}

    static DisplayStateInfo BuildMainMenuInfo(bool doAnimatedFadeIn) {
        DisplayStateInfo info;
        info.doAnimatedFadeIn = doAnimatedFadeIn;
        return info;
    }

    static DisplayStateInfo BuildSelectWorldInfo(int worldSelectionIdx) {
        DisplayStateInfo info;
        info.worldSelectionIdx = worldSelectionIdx;
        return info;
    }
    static DisplayStateInfo BuildWorldUnlockedInfo(int worldUnlockIdx) {
        DisplayStateInfo info;
        info.worldUnlockIdx = worldUnlockIdx;
        return info;
    }
    static DisplayStateInfo BuildSelectLevelInfo(int worldIdx, int levelSelectIdx = -1, bool doBasicUnlockAnim = false) {
        DisplayStateInfo info;
        info.worldSelectionIdx = worldIdx;
        info.levelSelectionIdx = levelSelectIdx;
        info.doBasicLevelUnlockAnim = doBasicUnlockAnim;
        return info;
    }

    int GetWorldSelectionIndex() const { return this->worldSelectionIdx; }
    int GetWorldUnlockIndex() const { return this->worldUnlockIdx; }
    int GetLevelSelectionIndex() const { return this->levelSelectionIdx; }
    bool GetDoBasicLevelUnlockAnimation() const { return this->doBasicLevelUnlockAnim; }
    bool GetDoAnimatedFadeIn() const { return this->doAnimatedFadeIn; }

private:
    int worldSelectionIdx;
    int worldUnlockIdx;
    int levelSelectionIdx;
    bool doBasicLevelUnlockAnim;
    bool doAnimatedFadeIn;
};

class DisplayState {

public:
	enum DisplayStateType { MainMenu, SelectWorldMenu, SelectLevelMenu, BlammopediaMenu, LevelStart,  
                            InTutorialGame, InGame, InGameBossLevel, InGameMenu, LevelEnd, LevelCompleteSummary,
                            BossLevelCompleteSummary, GameComplete, GameOver, Credits };

    DisplayState(GameDisplay* display);
    virtual ~DisplayState();

	static DisplayState* BuildDisplayStateFromType(const DisplayStateType& type, 
        const DisplayStateInfo& info, GameDisplay* display);
    static bool IsGameInPlayDisplayState(const DisplayStateType& type);

    virtual bool AllowsGameModelUpdates() const = 0;
	virtual void RenderFrame(double dT) = 0;

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, 
        const GameControl::ActionMagnitude& magnitude)   = 0;
	virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) = 0;
    virtual void MousePressed(const GameControl::MouseButton& pressedButton)     = 0;
    virtual void MouseReleased(const GameControl::MouseButton& releasedButton)    = 0;
    virtual void MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
        UNUSED_PARAMETER(oglWindowCoordX);
        UNUSED_PARAMETER(oglWindowCoordY);
    }
    virtual void WindowFocus(bool windowHasFocus) {
        UNUSED_PARAMETER(windowHasFocus);
    }

	virtual void DisplaySizeChanged(int width, int height) = 0;

	virtual DisplayState::DisplayStateType GetType() const = 0;

protected:
	GameDisplay* display;

#ifdef _DEBUG
	void DebugDrawBounds();
#endif

private:
	DISALLOW_COPY_AND_ASSIGN(DisplayState);
};

inline bool DisplayState::IsGameInPlayDisplayState(const DisplayStateType& type) {
    return (type == DisplayState::InGame || type == DisplayState::InGameBossLevel || type == DisplayState::InTutorialGame);
}

#endif