/**
 * GameOverDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEOVERDISPLAYSTATE_H__
#define __GAMEOVERDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

class GameDisplay;

class GameOverDisplayState : public DisplayState {

private:
	static const char* GAME_OVER_TEXT;
	TextLabel2D gameOverLabel;

public:
	GameOverDisplayState(GameDisplay* display);
	~GameOverDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;
};

inline void GameOverDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline void GameOverDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType GameOverDisplayState::GetType() const {
	return DisplayState::GameOver;
}

#endif
