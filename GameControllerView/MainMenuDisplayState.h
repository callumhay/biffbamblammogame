#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "DisplayState.h"

/**
 * State for displaying the main, out of game menu. This is the first 
 * game-related screen that the user will see.
 */
class MainMenuDisplayState : public DisplayState {

public:
	MainMenuDisplayState(GameDisplay* display);
	virtual ~MainMenuDisplayState();

	virtual void RenderFrame();
	virtual void KeyPressed(unsigned char key);

};

#endif