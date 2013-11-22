/**
 * DisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
    static DisplayStateInfo BuildSelectLevelInfo(int worldIdx, int levelSelectIdx = -1) {
        DisplayStateInfo info;
        info.worldSelectionIdx = worldIdx;
        info.levelSelectionIdx = levelSelectIdx;
        return info;
    }

    int GetWorldSelectionIndex() const { return this->worldSelectionIdx; }
    int GetWorldUnlockIndex() const { return this->worldUnlockIdx; }
    int GetLevelSelectionIndex() const { return this->levelSelectionIdx; }
    bool GetDoAnimatedFadeIn() const { return this->doAnimatedFadeIn; }

private:
    int worldSelectionIdx;
    int worldUnlockIdx;
    int levelSelectionIdx;
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