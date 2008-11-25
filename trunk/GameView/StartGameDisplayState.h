#ifndef __STARTGAMEDISPLAYSTATE_H__
#define __STARTGAMEDISPLAYSTATE_H__

#include "DisplayState.h"

class StartGameDisplayState : public DisplayState {

public:
	StartGameDisplayState(GameDisplay* display);
	virtual ~StartGameDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key);
	virtual void DisplaySizeChanged(int width, int height);
};

#endif