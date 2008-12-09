#ifndef __GAMEOVERDISPLAYSTATE_H__
#define __GAMEOVERDISPLAYSTATE_H__

#include "DisplayState.h"
#include "../BlammoEngine/BlammoEngine.h"

class GameOverDisplayState : public DisplayState {

private:
	static const std::string GAME_OVER_TEXT;
	TextLabel2D gameOverLabel;

public:
	GameOverDisplayState(GameDisplay* display);
	virtual ~GameOverDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(SDLKey key);
	virtual void DisplaySizeChanged(int width, int height);
};
#endif
