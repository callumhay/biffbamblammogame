/**
 * InGameDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "GameDisplay.h"
#include "DisplayState.h"
#include "InGameRenderPipeline.h"
#include "MouseRenderer.h"

class Texture2D;

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

public:
	InGameDisplayState(GameDisplay* display);
	virtual ~InGameDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	virtual void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };
    void MouseMoved(int oglWindowCoordX, int oglWindowCoordY);
    void WindowFocus(bool windowHasFocus);

	void DisplaySizeChanged(int width, int height);
	virtual DisplayState::DisplayStateType GetType() const;

protected:
	InGameRenderPipeline renderPipeline;

    virtual bool IsInGameMenuEnabled() const { return true; }

private:
	DISALLOW_COPY_AND_ASSIGN(InGameDisplayState);
};

inline void InGameDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline void InGameDisplayState::MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
    this->display->GetMouse()->ShowMouse(oglWindowCoordX, oglWindowCoordY);
}

inline void InGameDisplayState::WindowFocus(bool windowHasFocus) {
    this->display->GetMouse()->SetWindowHasFocus(windowHasFocus);
}

inline DisplayState::DisplayStateType InGameDisplayState::GetType() const {
	return DisplayState::InGame;
}

#endif