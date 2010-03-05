#ifndef __DISPLAYSTATE_H__
#define __DISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameDisplay;

class DisplayState {

protected:
	GameDisplay* display;

public:
	DisplayState(GameDisplay* display) : display(display) {}
	virtual ~DisplayState() {};

	virtual void RenderFrame(double dT) = 0;
	virtual void KeyPressed(SDLKey key, SDLMod modifier) = 0;
	virtual void KeyReleased(SDLKey key, SDLMod modifier) = 0;
	virtual void DisplaySizeChanged(int width, int height) = 0;

};

#endif