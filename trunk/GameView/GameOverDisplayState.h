#ifndef __GAMEOVERDISPLAYSTATE_H__
#define __GAMEOVERDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

class GameDisplay;

class GameOverDisplayState : public DisplayState {

private:
	static const std::string GAME_OVER_TEXT;
	TextLabel2D gameOverLabel;

public:
	GameOverDisplayState(GameDisplay* display);
	~GameOverDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);
};
#endif
