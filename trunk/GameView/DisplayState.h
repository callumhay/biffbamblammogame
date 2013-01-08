/**
 * DisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __DISPLAYSTATE_H__
#define __DISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameControl/GameControl.h"

class GameDisplay;

class DisplayState {

public:
	enum DisplayStateType { MainMenu, SelectWorldMenu, SelectLevelMenu, BlammopediaMenu, LevelStart, WorldStart, 
                            InTutorialGame, InGame, InGameMenu, LevelEnd, LevelCompleteSummary, GameComplete, GameOver };
	static DisplayState* BuildDisplayStateFromType(const DisplayStateType& type, GameDisplay* display);

	DisplayState(GameDisplay* display) : display(display) {}
	virtual ~DisplayState() {};

    virtual bool AllowsGameModelUpdates() const = 0;
	virtual void RenderFrame(double dT) = 0;

	virtual void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude)   = 0;
	virtual void ButtonReleased(const GameControl::ActionButton& releasedButton) = 0;
    virtual void MousePressed(const GameControl::MouseButton& pressedButton)     = 0;
    virtual void MouseReleased(const GameControl::MouseButton& releasedButton)    = 0;

	virtual void DisplaySizeChanged(int width, int height) = 0;

	virtual DisplayState::DisplayStateType GetType() const = 0;

protected:
	GameDisplay* display;

    void DrawFadeOverlay(int width, int height, float alpha);

#ifdef _DEBUG
	void DebugDrawBounds();
#endif

private:
	DISALLOW_COPY_AND_ASSIGN(DisplayState);

};

inline void DisplayState::DrawFadeOverlay(int width, int height, float alpha) {
    // Draw the fade quad overlay
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GeometryMaker::GetInstance()->DrawFullScreenQuad(width, height, 1.0f, 
                                                     ColourRGBA(0, 0, 0, alpha));
    glPopAttrib();
}

#endif