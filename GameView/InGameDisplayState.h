/**
 * InGameDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "DisplayState.h"
#include "InGameRenderPipeline.h"

class Texture2D;

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

public:
	InGameDisplayState(GameDisplay* display);
	~InGameDisplayState();

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
	InGameRenderPipeline renderPipeline;

#ifdef _DEBUG
	void DebugDrawBounds();
#endif

	DISALLOW_COPY_AND_ASSIGN(InGameDisplayState);
};

inline void InGameDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline DisplayState::DisplayStateType InGameDisplayState::GetType() const {
	return DisplayState::InGame;
}

#endif