#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/BlammoEngine.h"

class GameMenu;

/**
 * State for displaying the main, out of game menu. This is the first 
 * game-related screen that the user will see.
 */
class MainMenuDisplayState : public DisplayState {

private:
	// Title and menu text
	static const std::string TITLE_TEXT;
	static const std::string NEW_GAME_MENUITEM;
	static const std::string PLAY_LEVEL_MENUITEM;
	static const std::string OPTIONS_MENUITEM;
	static const std::string EXIT_MENUITEM;

	// Indices for the menu
	static const int NEW_GAME_INDEX		= 0;
	static const int PLAY_LEVEL_INDEX = 1;
	static const int OPTIONS_INDEX		= 2;
	static const int EXIT_INDEX				= 3;

	// Indents and spacing
	static const unsigned int MENU_X_INDENT = 50;
	static const unsigned int MENU_Y_INDENT = 100;
	static const unsigned int TITLE_Y_INDENT = 30;
	static const unsigned int MENU_ITEM_PADDING = 15;

	TextLabel2D* titleLabel;
	GameMenu* menu;
	void InitializeMenu();

public:
	MainMenuDisplayState(GameDisplay* display);
	virtual ~MainMenuDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key);
	virtual void DisplaySizeChanged(int width, int height);
};

#endif