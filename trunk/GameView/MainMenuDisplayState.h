/**
 * MainMenuDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"

#include "GameMenu.h"
#include "DisplayState.h"

class TextLabel2D;
class FBObj;
class CgFxBloom;

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

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;

	// Indices for the main menu
	int newGameMenuItemIndex;
	int playLevelMenuItemIndex;
	int optionsMenuItemIndex;
	int exitGameMenuItemIndex;

	// Indices for the options submenu
	int optionsBackIndex;
	int optionsFullscreenIndex;
	int optionsResolutionIndex;

	// Indents and spacing
	static const unsigned int MENU_X_INDENT = 50;
	static const unsigned int MENU_Y_INDENT = 100;
	static const unsigned int TITLE_Y_INDENT = 30;
	static const unsigned int MENU_ITEM_PADDING = 15;

	CgFxBloom* bloomEffect;
	FBObj* menuFBO;


	TextLabel2D* titleLabel;
	GameMenu* mainMenu;
	GameSubMenu* optionsSubMenu;

	AnimationLerp<float> fadeAnimation;	// Animation for fading in/out of the menu screen - represents alpha fade value

	bool changeToPlayGameState;

	void InitializeMainMenu();
	void InitializeOptionsSubMenu();

	void SetupBloomEffect();

public:
	MainMenuDisplayState(GameDisplay* display);
	virtual ~MainMenuDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(SDLKey key);
	virtual void DisplaySizeChanged(int width, int height);

	class MainMenuEventHandler : public GameMenuEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		MainMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		virtual ~MainMenuEventHandler() {}
		virtual void GameMenuItemActivatedEvent(int itemIndex);
		virtual void EscMenu();
	};

	class OptionsSubMenuEventHandler : public GameMenuEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		OptionsSubMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		virtual ~OptionsSubMenuEventHandler() {}
		virtual void GameMenuItemActivatedEvent(int itemIndex);
		virtual void EscMenu();
	};

private:
	MainMenuEventHandler* mainMenuEventHandler;						// Event handler for the main (top-most) menu
	OptionsSubMenuEventHandler* optionsMenuEventHandler;	// Event handler for the options sub-menu
};

#endif