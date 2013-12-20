/**
 * MainMenuDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __MAINMENUDISPLAYSTATE_H__
#define __MAINMENUDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"

#include "../ESPEngine/ESPPointEmitter.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPMultiColourEffector.h"
#include "../ESPEngine/ESPEmitterEventHandler.h"

#include "GameMenu.h"
#include "DisplayState.h"
#include "DecoratorOverlayPane.h"
#include "PopupTutorialHint.h"
#include "BBBTitleDisplay.h"

#include "../ConfigOptions.h"

class TextLabel2D;
class FBObj;

/**
 * State for displaying the main, out of game menu. This is the first 
 * game-related screen that the user will see.
 */
class MainMenuDisplayState : public DisplayState {

private:
	// Menu text
	static const char* NEW_GAME_MENUITEM;
    static const char* CONTINUE_MENUITEM;
	static const char* PLAY_LEVEL_MENUITEM;
    static const char* BLAMMOPEDIA_MENUITEM;
	static const char* OPTIONS_MENUITEM;
    static const char* CREDITS_MENUITEM;
	static const char* EXIT_MENUITEM;

	static const Colour MENU_ITEM_IDLE_COLOUR;
	static const Colour MENU_ITEM_SEL_COLOUR;
	static const Colour MENU_ITEM_ACTIVE_COLOUR;
	static const Colour MENU_ITEM_GREYED_COLOUR;
	static const Colour SUBMENU_ITEM_IDLE_COLOUR;
	static const Colour SUBMENU_ITEM_ACTIVE_COLOUR;

	static const int MENU_SEL_ON_INDEX;
	static const int MENU_SEL_OFF_INDEX;

	static const float CAM_DIST_FROM_ORIGIN;

    static const double FADE_IN_TIME_IN_SECS;
    static const double FADE_OUT_TIME_IN_SECS;

	// The configuration options for the game
	ConfigOptions cfgOptions;

	// Indices for the main menu
	int startGameMenuItemIndex;
	int playLevelMenuItemIndex;
	int optionsMenuItemIndex;
    int blammopediaItemIndex;
    int creditsItemIndex;
	int exitGameMenuItemIndex;

	// Indices for the options submenu
	int optionsBackIndex;
	int optionsFullscreenIndex;
	int optionsVSyncIndex;
	int optionsResolutionIndex;
	int optionsMusicVolumeIndex;
    int optionsSFXVolumeIndex;
    //int optionsControllerSensitivityIndex;
    int optionsInvertBallBoostIndex;
    int optionsBallBoostModeIndex;
    int optionsDifficultyIndex;
    int optionsEraseProgressIndex;

    PopupTutorialHint* eraseSuccessfulPopup;
    PopupTutorialHint* eraseFailedPopup;

	// Indents and spacing
	static const unsigned int MENU_X_INDENT = 50;
	static const unsigned int MENU_Y_INDENT = 320;
	static const unsigned int MENU_ITEM_PADDING = 15;

	FBObj* menuFBO;
    FBObj* postMenuFBObj;

    TextLabel2D madeByTextLabel;
    TextLabel2D licenseLabel;

	ESPMultiColourEffector particleFadeInAndOut;
	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;

	std::vector<Texture*> bangTextures;
	
    BBBTitleDisplay titleDisplay;

	GameMenu* mainMenu;             // Main (top-most/parent) menu
	GameSubMenu* optionsSubMenu;    // Options sub-menu

    GameMenuItem* startGameMenuItem;

	// Pointers for tracking the menu items in the options menu
	SelectionListMenuItem*  fullscreenMenuItem;
	SelectionListMenuItem*  resolutionMenuItem;
	SelectionListMenuItem*  vSyncMenuItem;
	AmountScrollerMenuItem* musicVolumeMenuItem;
    AmountScrollerMenuItem* sfxVolumeMenuItem;
    SelectionListMenuItem*  invertBallBoostItem;
    SelectionListMenuItem*  ballBoostModeItem;
    SelectionListMenuItem*  difficultyItem;
    //AmountScrollerMenuItem* controllerSensitivityItem;

    bool doAnimatedFadeIn;
	AnimationLerp<float> fadeAnimation;	// Animation for fading in/out of the menu screen - represents alpha fade value

	bool changeToPlayGameState;
    bool changeToBlammopediaState;
    bool changeToLevelSelectState;
    bool changeToCreditsState;

    //TODO? static const int BG_SOUND_FADEOUT_IN_MILLISECS = 1000;
    SoundID bgLoopedSoundID;

	void InitializeESPEffects();
	void InitializeMainMenu();
	void InitializeOptionsSubMenu();

	void RenderBackgroundEffects(double dT, Camera& menuCam);

	void InsertBangEffectIntoBGEffects(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

	static const size_t TOTAL_NUM_BANG_EFFECTS;
	std::list<ESPPointEmitter*> aliveBangEffects;
	std::list<ESPPointEmitter*> aliveOnoEffects;
	std::list<ESPPointEmitter*> deadBangEffects;
	std::list<ESPPointEmitter*> deadOnoEffects;

public:
	MainMenuDisplayState(GameDisplay* display, const DisplayStateInfo& info = DisplayStateInfo());
	~MainMenuDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

	class MainMenuEventHandler : public GameMenuEventHandler {
	public:
		MainMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~MainMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void EscMenu();
	private:
		MainMenuDisplayState* mainMenuState;
	};

	class OptionsSubMenuEventHandler : public GameMenuEventHandler {
	public:
		OptionsSubMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~OptionsSubMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void EscMenu();
	private:
		MainMenuDisplayState* mainMenuState;
	};

	class SelectListItemsEventHandler : public SelectionListEventHandlerWithSound {
	public:
		SelectListItemsEventHandler(MainMenuDisplayState* mainMenuState);
		~SelectListItemsEventHandler() {}

	private:
		MainMenuDisplayState* mainMenuState;
	};

    class VolumeItemEventHandler : public ScrollerItemEventHandlerWithSound {
	public:
        enum Type { SFX, Music };

		VolumeItemEventHandler(MainMenuDisplayState* mainMenuState, Type t);
		~VolumeItemEventHandler() {}

        void MenuItemScrolled();
        void MenuItemConfirmed();
        void MenuItemCancelled();

	private:
        Type type;

        void UpdateGameMasterVolume(int volumeLevel);
		MainMenuDisplayState* mainMenuState;
    };

    class QuitVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
	public:
        QuitVerifyEventHandler(MainMenuDisplayState *mainMenuState);
		~QuitVerifyEventHandler() {}

		void MenuItemConfirmed();

	private:
		MainMenuDisplayState* mainMenuState;
    };

    class EraseProgressVerifyEventHandler : public VerifyMenuEventHandlerWithSound {
	public:
        EraseProgressVerifyEventHandler(MainMenuDisplayState *mainMenuState);
		~EraseProgressVerifyEventHandler() {}

		void MenuItemConfirmed();

	private:
		MainMenuDisplayState* mainMenuState;
    };

	class BangParticleEventHandler : public ESPEmitterEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		BangParticleEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~BangParticleEventHandler() {}

		void ParticleSpawnedEvent(const ESPParticle* particle);
        void ParticleDiedEvent(const ESPParticle*) {}
	};

private:
	MainMenuEventHandler* mainMenuEventHandler;             // Event handler for the main (top-most) menu
	OptionsSubMenuEventHandler* optionsMenuEventHandler;	// Event handler for the options sub-menu
	SelectListItemsEventHandler* selectListItemsHandler;            // Event handler for more complicated items (e.g., scrollers, verify menus)
	VolumeItemEventHandler* musicVolItemHandler;
    VolumeItemEventHandler* sfxVolItemHandler;
    BangParticleEventHandler* particleEventHandler;         // Event handler for the sound particles
    QuitVerifyEventHandler* quitVerifyHandler;
    EraseProgressVerifyEventHandler* eraseProgVerifyHandler;

	DISALLOW_COPY_AND_ASSIGN(MainMenuDisplayState);
};

inline DisplayState::DisplayStateType MainMenuDisplayState::GetType() const {
	return DisplayState::MainMenu;
}

#endif