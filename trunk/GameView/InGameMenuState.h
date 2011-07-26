/**
 * InGameMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INGAMEMENUSTATE_H__
#define __INGAMEMENUSTATE_H__

#include "DisplayState.h"
#include "GameMenu.h"
#include "InGameRenderPipeline.h"

class InGameDisplayState;

/**
 * State for displaying the in-game menu (i.e., when you pause the game while
 * playing it and change various options or go back to the main menu).
 */
class InGameMenuState : public DisplayState {

public:
	InGameMenuState(GameDisplay* display);
	~InGameMenuState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;

	enum NextAction { Nothing, ResumeGame, RestartLevel, ReturnToMainMenu, ExitToDesktop };
	NextAction nextAction;

    InGameRenderPipeline renderPipeline;

	// Top Level Menu variables
	GameMenu* topMenu; // Main (top-most/parent) menu (features options "Return to Main Menu", etc.)
	
    int resumeItem;
    int restartItem;
	//int audioMenu;
	int returnToMainItem;
	int exitToDesktopItem;

	void InitTopMenu();

	void ResumeTheGame();

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

	DISALLOW_COPY_AND_ASSIGN(InGameMenuState);
};

inline void InGameMenuState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType InGameMenuState::GetType() const {
	return DisplayState::InGameMenu;
}

#endif // __INGAMEMENUSTATE_H__