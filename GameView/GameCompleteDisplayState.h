/**
 * GameCompleteDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMECOMPLETEDISPLAYSTATE_H__
#define __GAMECOMPLETEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

/**
 * State for displaying the completion of the game animation and
 * end credits.
 */
class GameCompleteDisplayState : public DisplayState {

private:
	static const char* GAME_COMPLETE_TEXT;
	TextLabel2D gameCompleteLabel;
	float gameCompleteLabelRasterWidth;

public:
	GameCompleteDisplayState(GameDisplay* display);
	~GameCompleteDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;
};

inline void GameCompleteDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}
inline void GameCompleteDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType GameCompleteDisplayState::GetType() const {
	return DisplayState::GameComplete;
}

#endif