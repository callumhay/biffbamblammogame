/**
 * InGameMenuState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __INGAMEMENUSTATE_H__
#define __INGAMEMENUSTATE_H__

#include "DisplayState.h"
#include "GameMenu.h"
#include "InGameRenderPipeline.h"
#include "DecoratorOverlayPane.h"
#include "PopupTutorialHint.h"

#include "../ConfigOptions.h"

class InGameDisplayState;

/**
 * State for displaying the in-game menu (i.e., when you pause the game while
 * playing it and change various options or go back to the main menu).
 */
class InGameMenuState : public DisplayState {

public:
	InGameMenuState(GameDisplay* display, DisplayState* returnToDisplayState);
	~InGameMenuState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;
    
    void CleanUpReturnToDisplayState();

private:

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;

    static const char* VERIFY_MENU_YES;
    static const char* VERIFY_MENU_NO;
    
    static const int LEVEL_NAME_LABEL_X_BORDER;
    static const int MENU_LABEL_GAP;
    static const int NAME_HS_LABEL_GAP;
    static const int LABEL_STAR_GAP;
    static const int STAR_SIZE;
    static const int STAR_HORIZONTAL_GAP;
    static const int BOSS_ICON_SIZE;

	enum NextAction { Nothing, ResumeGame, RestartLevel, ReturnToMainMenu, ExitToDesktop };
	NextAction nextAction;

    InGameRenderPipeline renderPipeline;

    DisplayState* returnToDisplayState;

	// The configuration options for the game
	ConfigOptions cfgOptions;

    // Text labels for the level name, stars, and high score
    TextLabel2DFixedWidth levelNameLabel;
    TextLabel2D highscoreLabel;
    Texture* starTexture;
    Texture* bossTexture;
    AnimationMultiLerp<Colour> activeStarColourAnim;

	// Top Level Menu variables
	GameMenu* topMenu; // Main (top-most/parent) menu (features options "Return to Main Menu", etc.)
	
    int resumeItem;
    int restartItem;
    int difficultyItem;
	int returnToMainItem;
	int exitToDesktopItem;
    int initialDifficultySelected;

    SelectionListMenuItemWithVerify* difficultyMenuItem;

    void DrawStars(double dT, float currYPos);
    void DrawBossIcon(float currYPos);

	void InitTopMenu();
	void ResumeTheGame();
    
	// Top Level Menu Handler class
	class TopMenuEventHandler : public GameMenuEventHandler {
	public:
		TopMenuEventHandler(InGameMenuState* inGameMenuState) : inGameMenuState(inGameMenuState) {}
		~TopMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void EscMenu();

	private:
		InGameMenuState* inGameMenuState;
	};

	// Event handlers for verify menus
	class RestartVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
	public:
		RestartVerifyEventHandler(InGameMenuState* inGameMenuState);
		~RestartVerifyEventHandler() {}

		void MenuItemConfirmed();

	private:
		InGameMenuState* inGameMenuState;
	};
	class ExitGameVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
	public:
		ExitGameVerifyEventHandler(InGameMenuState* inGameMenuState);
		~ExitGameVerifyEventHandler() {}

		void MenuItemConfirmed();

	private:
		InGameMenuState* inGameMenuState;
	};

	class ReturnToMainMenuVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
	public:
		ReturnToMainMenuVerifyEventHandler(InGameMenuState* inGameMenuState);
		~ReturnToMainMenuVerifyEventHandler() {}

		void MenuItemConfirmed();

	private:
		InGameMenuState* inGameMenuState;
	};

    class DifficultyEventHandler : public SelectionListEventHandlerWithSound {
    public:
        DifficultyEventHandler(InGameMenuState* inGameMenuState);

	private:
		InGameMenuState* inGameMenuState;
    };

    class DifficultyVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
    public:
        DifficultyVerifyEventHandler(InGameMenuState* inGameMenuState);
        void MenuItemConfirmed();
        void MenuItemCancelled();

    private:
		InGameMenuState* inGameMenuState;
    };

	TopMenuEventHandler* topMenuEventHandler;
    DifficultyEventHandler* difficultyEventHandler;
    DifficultyVerifyEventHandler* difficultyVerifyHandler;
    RestartVerifyEventHandler* restartVerifyHandler;
    ExitGameVerifyEventHandler* exitGameVerifyHandler;
    ReturnToMainMenuVerifyEventHandler* returnToMainMenuVerifyHandler;

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