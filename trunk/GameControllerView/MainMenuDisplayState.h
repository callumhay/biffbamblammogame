#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../Utils/Point.h"

class GameMenu;

/**
 * State for displaying the main, out of game menu. This is the first 
 * game-related screen that the user will see.
 */
class MainMenuDisplayState : public DisplayState {
private:
	static Point2D MENU_TOPLEFT_CORNER;
	static const unsigned int MENU_ITEM_PADDING = 1;

	GameMenu* menu;
	void InitializeMenu();

public:
	MainMenuDisplayState(GameDisplay* display);
	virtual ~MainMenuDisplayState();

	virtual void RenderFrame();
	virtual void KeyPressed(unsigned char key);

};

#endif