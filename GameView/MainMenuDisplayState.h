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

#include "../ESPEngine/ESPPointEmitter.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPMultiColourEffector.h"
#include "../ESPEngine/ESPEmitterEventHandler.h"

#include "GameMenu.h"
#include "DisplayState.h"

#include "../ConfigOptions.h"

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
	static const char* TITLE_TEXT;
	static const char* TITLE_BIFF_TEXT;
	static const char* TITLE_BAM_TEXT;
	static const char* TITLE_BLAMMO_TEXT;

	static const char* NEW_GAME_MENUITEM;
	static const char* PLAY_LEVEL_MENUITEM;
	static const char* OPTIONS_MENUITEM;
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

	// The configuration options for the game
	ConfigOptions cfgOptions;

	// Indices for the main menu
	int newGameMenuItemIndex;
	int playLevelMenuItemIndex;
	int optionsMenuItemIndex;
	int exitGameMenuItemIndex;

	// Indices for the options submenu
	int optionsBackIndex;
	int optionsFullscreenIndex;
	int optionsVSyncIndex;
	int optionsResolutionIndex;
	int optionsSoundVolumeIndex;

	// Indents and spacing
	static const unsigned int MENU_X_INDENT = 50;
	static const unsigned int MENU_Y_INDENT = 320;
	static const unsigned int MENU_ITEM_PADDING = 15;

	float biffTitleWidth, bamTitleWidth, blammoTitleWidth;

	CgFxBloom* bloomEffect;
	FBObj* menuFBO;

	// Particle-related variables
	ESPPointEmitter biffEmitter, bamEmitter, blammoEmitter;
	ESPPointEmitter biffTextEmitter, bamTextEmitter, blammoTextEmitter;
	
	//ESPParticleColourEffector particleFader;
	ESPMultiColourEffector particleFadeInAndOut;
	ESPParticleScaleEffector particleSmallGrowth;
	ESPParticleScaleEffector particleMediumGrowth;


	std::vector<Texture*> bangTextures;
	
	GameMenu* mainMenu;						// Main (top-most/parent) menu
	GameSubMenu* optionsSubMenu;	// Options sub-menu

	// Pointers for tracking the menu items in the options menu
	SelectionListMenuItem* fullscreenMenuItem;
	SelectionListMenuItem* resolutionMenuItem;
	SelectionListMenuItem* vSyncMenuItem;
	AmountScrollerMenuItem* soundVolumeMenuItem;

	// 

	AnimationLerp<float> fadeAnimation;	// Animation for fading in/out of the menu screen - represents alpha fade value

	bool changeToPlayGameState;

	void InitializeESPEffects();
	void InitializeMainMenu();
	void InitializeOptionsSubMenu();

	void SetupBloomEffect();

	void RenderTitle(Camera& menuCam);
	void RenderBackgroundEffects(double dT, Camera& menuCam);

	void InsertBangEffectIntoBGEffects(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
	//void InsertZapEffectIntoBGEffects();

	static const size_t TOTAL_NUM_BANG_EFFECTS;
	std::list<ESPPointEmitter*> aliveBangEffects;
	std::list<ESPPointEmitter*> aliveOnoEffects;
	std::list<ESPPointEmitter*> deadBangEffects;
	std::list<ESPPointEmitter*> deadOnoEffects;

public:
	MainMenuDisplayState(GameDisplay* display);
	~MainMenuDisplayState();

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

	class MainMenuEventHandler : public GameMenuEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		MainMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~MainMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void GameMenuItemVerifiedEvent(int itemIndex);
		void EscMenu();
	};

	class OptionsSubMenuEventHandler : public GameMenuEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		OptionsSubMenuEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~OptionsSubMenuEventHandler() {}
		void GameMenuItemHighlightedEvent(int itemIndex);
		void GameMenuItemActivatedEvent(int itemIndex);
		void GameMenuItemChangedEvent(int itemIndex);
		void GameMenuItemVerifiedEvent(int itemIndex) {
			UNUSED_PARAMETER(itemIndex);
		};
		void EscMenu();
	};

	class AllMenuItemsEventHandler : public GameMenuItemEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		AllMenuItemsEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~AllMenuItemsEventHandler() {}

		void MenuItemScrolled();
		void MenuItemEnteredAndSet();
		void MenuItemCancelled();
	};

	class BangParticleEventHandler : public ESPEmitterEventHandler {
	private:
		MainMenuDisplayState* mainMenuState;
	public:
		BangParticleEventHandler(MainMenuDisplayState *mainMenuState) : mainMenuState(mainMenuState) {}
		~BangParticleEventHandler() {}

		void ParticleSpawnedEvent(const ESPParticle* particle); 
	};

private:
	MainMenuEventHandler* mainMenuEventHandler;						// Event handler for the main (top-most) menu
	OptionsSubMenuEventHandler* optionsMenuEventHandler;	// Event handler for the options sub-menu
	AllMenuItemsEventHandler* itemsEventHandler;					// Event handler for more complicated items (e.g., scrollers, verify menus)
	BangParticleEventHandler* particleEventHandler;				// Event handler for the sound particles

	DISALLOW_COPY_AND_ASSIGN(MainMenuDisplayState);
};

inline DisplayState::DisplayStateType MainMenuDisplayState::GetType() const {
	return DisplayState::MainMenu;
}

#endif