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

class DisplayStateInfo {
public:
    DisplayStateInfo() : worldSelectionIdx(-1), worldUnlockIdx(-1), levelSelectionIdx(-1) {}

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

private:
    int worldSelectionIdx;
    int worldUnlockIdx;
    int levelSelectionIdx;
};

class DisplayState {

public:
	enum DisplayStateType { MainMenu, SelectWorldMenu, SelectLevelMenu, BlammopediaMenu, LevelStart, WorldStart, 
                            InTutorialGame, InGame, InGameBossLevel, InGameMenu, LevelEnd, LevelCompleteSummary,
                            BossLevelCompleteSummary, GameComplete, GameOver, Credits };

	static DisplayState* BuildDisplayStateFromType(const DisplayStateType& type, const DisplayStateInfo& info, GameDisplay* display);
    static bool IsGameInPlayDisplayState(const DisplayStateType& type);

	DisplayState(GameDisplay* display) : display(display) {}
	virtual ~DisplayState() {};

    virtual bool AllowsGameModelUpdates() const = 0;
	virtual void RenderFrame(double dT) = 0;

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude)   = 0;
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

    void DrawFadeOverlay(int width, int height, float alpha);
    void DrawFadeOverlayWithTex(int width, int height, float alpha, const Texture2D* tex);

#ifdef _DEBUG
	void DebugDrawBounds();
#endif

private:
	DISALLOW_COPY_AND_ASSIGN(DisplayState);

};

inline bool DisplayState::IsGameInPlayDisplayState(const DisplayStateType& type) {
    return (type == DisplayState::InGame || type == DisplayState::InGameBossLevel || type == DisplayState::InTutorialGame);
}

inline void DisplayState::DrawFadeOverlay(int width, int height, float alpha) {
    // Draw the fade quad overlay
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GeometryMaker::GetInstance()->DrawFullScreenQuad(width, height, 1.0f, ColourRGBA(0, 0, 0, alpha));
    glPopAttrib();
}

inline void DisplayState::DrawFadeOverlayWithTex(int width, int height, float alpha, const Texture2D* tex) {
    // Draw the fade quad overlay
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    tex->BindTexture();
    GeometryMaker::GetInstance()->DrawTiledFullScreenQuad(width, height, 
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(width) / static_cast<float>(tex->GetWidth()),
        GameViewConstants::STARRY_BG_TILE_MULTIPLIER * static_cast<float>(height) / static_cast<float>(tex->GetHeight()),
        ColourRGBA(1,1,1, alpha));
    tex->UnbindTexture();
    glPopAttrib();
}

#endif