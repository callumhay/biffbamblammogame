#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "DisplayState.h"

class GameMenu;
class TextLabel2D;

/**
 * State for displaying the main, out of game menu. This is the first 
 * game-related screen that the user will see.
 */
class MainMenuDisplayState : public DisplayState {

private:
	// Title and menu text
	static const std::string TITLE_TEXT;
	static const std::string TITLE_BIFF_TEXT;
	static const std::string TITLE_BAM_TEXT;
	static const std::string TITLE_BLAMMO_TEXT;

	static const std::string NEW_GAME_MENUITEM;
	static const std::string PLAY_LEVEL_MENUITEM;
	static const std::string OPTIONS_MENUITEM;
	static const std::string EXIT_MENUITEM;

	// Indices for the menu
	int newGameMenuItemIndex;
	int playLevelMenuItemIndex;
	int optionsMenuItemIndex;
	int exitGameMenuItemIndex;

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
	virtual void KeyPressed(SDLKey key);
	virtual void DisplaySizeChanged(int width, int height);
};

#endif