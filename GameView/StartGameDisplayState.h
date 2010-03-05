#ifndef __STARTGAMEDISPLAYSTATE_H__
#define __STARTGAMEDISPLAYSTATE_H__

#include "DisplayState.h"

class StartGameDisplayState : public DisplayState {

public:
	StartGameDisplayState(GameDisplay* display);
	~StartGameDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);
};

#endif