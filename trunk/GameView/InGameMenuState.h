#ifndef __INGAMEMENUSTATE_H__
#define __INGAMEMENUSTATE_H__

#include "DisplayState.h"
#include "GameMenu.h"

class InGameDisplayState;


/**
 * State for displaying the in-game menu (i.e., when you pause the game while
 * playing it and change various options or go back to the main menu).
 */
class InGameMenuState : public DisplayState {

public:
	InGameMenuState(GameDisplay* display);
	~InGameMenuState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);

private:

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;

	enum NextAction { Nothing, ResumeGame, ReturnToMainMenu, ExitToDesktop };
	NextAction nextAction;

	// Top Level Menu variables
	GameMenu* topMenu; // Main (top-most/parent) menu (features options "Return to Main Menu", etc.)
	int resumeItem;
	//int audioMenu;
	int returnToMainItem;
	int exitToDesktopItem;

	void InitTopMenu();

	// Top Level Menu Handler class
	class TopMenuEventHandler : public GameMenuEventHandler {
	private:
		InGameMenuState* inGameMenuState;
	public:
		TopMenuEventHandler(InGameMenuState *inGameMenuState) : inGameMenuState(inGameMenuState) {}
		~TopMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void GameMenuItemVerifiedEvent(int itemIndex);
		void EscMenu();
	};

	// Event handler for verify menus
	class VerifyMenuEventHandler : public GameMenuItemEventHandler {
	private:
		InGameMenuState* inGameMenuState;
	public:
		VerifyMenuEventHandler(InGameMenuState *inGameMenuState) : inGameMenuState(inGameMenuState) {}
		~VerifyMenuEventHandler() {}

		void MenuItemScrolled();
		void MenuItemEnteredAndSet();
		void MenuItemCancelled();
	};

	TopMenuEventHandler* topMenuEventHandler;
	VerifyMenuEventHandler* verifyMenuEventHandler;

};

#endif // __INGAMEMENUSTATE_H__