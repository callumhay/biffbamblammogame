/**
 * InGameMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

// BlammoEngine Includes
#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"

// GameDisplay Includes
#include "InGameMenuState.h"
#include "InGameDisplayState.h"
#include "MainMenuDisplayState.h"
#include "LevelStartDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"

// GameModel Includes
#include "../GameModel/GameModel.h"

// GameSound Includes
#include "../GameSound/GameSound.h"

const Colour InGameMenuState::MENU_ITEM_IDLE_COLOUR		= Colour(0.75f, 0.75f, 0.75f);
const Colour InGameMenuState::MENU_ITEM_SEL_COLOUR		= Colour(1, 0.8f, 0);
const Colour InGameMenuState::MENU_ITEM_ACTIVE_COLOUR	= Colour(1.0f, 1.0f, 1.0f);
const Colour InGameMenuState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);

const char* InGameMenuState::VERIFY_MENU_YES  = "Yes!";
const char* InGameMenuState::VERIFY_MENU_NO   = "NOoo!";

InGameMenuState::InGameMenuState(GameDisplay* display, DisplayState* returnToDisplayState) : 
DisplayState(display), renderPipeline(display), nextAction(InGameMenuState::Nothing),
topMenu(NULL), topMenuEventHandler(NULL), difficultyEventHandler(NULL), difficultyVerifyHandler(NULL),
restartVerifyHandler(NULL), exitGameVerifyHandler(NULL), returnToMainMenuVerifyHandler(NULL),
returnToDisplayState(returnToDisplayState), initialDifficultySelected(-1) {

    GameSound* sound = this->display->GetSound();
        
	// Pause all world sounds
	sound->PauseAllSounds();
    // ... but unpause the looped background music, and set its volume to about half
    // TODO
    // ..and play the paused sound
    sound->PlaySound(GameSound::InGameMenuOpened, false);

	// Pause the game itself
	this->display->GetModel()->SetPause(GameModel::PauseGame);

	// Read the configuration file to figure out how to initialize each of the options
	this->cfgOptions = ResourceManager::ReadConfigurationOptions(true);

	this->InitTopMenu();
}

InGameMenuState::~InGameMenuState() {
	delete this->topMenu;
	this->topMenu = NULL;
	delete this->topMenuEventHandler;
	this->topMenuEventHandler = NULL;
    delete this->difficultyEventHandler;
    this->difficultyEventHandler = NULL;
    delete this->difficultyVerifyHandler;
    this->difficultyVerifyHandler = NULL;
    delete this->restartVerifyHandler;
    this->restartVerifyHandler = NULL;
    delete this->exitGameVerifyHandler;
    this->exitGameVerifyHandler = NULL;
    delete this->returnToMainMenuVerifyHandler;
    this->returnToMainMenuVerifyHandler = NULL;
}

/**
 * Render a frame - this will render the last frame from the game with an overlay of the
 * in-game menu in its current state according to interaction with the user.
 */
void InGameMenuState::RenderFrame(double dT) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (this->nextAction) {

		case InGameMenuState::ResumeGame:
			this->ResumeTheGame();
			return;

        case InGameMenuState::RestartLevel:
            // Clean up any misc. visual effects
			this->display->GetAssets()->DeactivateMiscEffects();
			
            // Kill all sounds
			this->display->GetSound()->StopAllSounds();
            this->display->GetSound()->StopAllEffects();

            // Reset the level
            this->display->GetModel()->ResetCurrentLevel();

            // Un-Pause the game and go to the start of level state which will have
            // been queued when we told the model to reset the level
            this->CleanUpReturnToDisplayState();
            
            this->display->SetCurrentStateAsNextQueuedState();

            return;

		case InGameMenuState::ReturnToMainMenu:
            // Clean up any misc. visual effects
			this->display->GetAssets()->DeactivateMiscEffects();
			
            // Kill all sounds
			this->display->GetSound()->StopAllSounds();
            this->display->GetSound()->StopAllEffects();
            
			// Go back to the main menu state
            this->CleanUpReturnToDisplayState();
			this->display->SetCurrentState(new MainMenuDisplayState(this->display));
			return;

		case InGameMenuState::ExitToDesktop:
            this->CleanUpReturnToDisplayState();
			this->display->QuitGame();
			return;

		case InGameMenuState::Nothing:
			break;

		default:
			assert(false);
			break;
	}

	// Render the last frame from the in-game display state, stored in the FBO assets
	// this will be the 'background' for the menu...
	FBObj* lastFullscreenRendering = this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO();
	assert(lastFullscreenRendering != NULL);

	lastFullscreenRendering->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);
    renderPipeline.RenderHUD(0.0);

	// Render an overlay that dulls the background rendering...
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(),
        -1.0f, ColourRGBA(0.0f, 0.0f, 0.0f, 0.5f));
	
	// Draw the menus...
	glEnable(GL_DEPTH_TEST);
	this->topMenu->SetCenteredOnScreen(Camera::GetWindowWidth(), Camera::GetWindowHeight());
	this->topMenu->Draw(dT, Camera::GetWindowWidth(), Camera::GetWindowHeight());
	glPopAttrib();
}

void InGameMenuState::ResumeTheGame() {
    GameSound* sound = this->display->GetSound();
    
    // Play unpause sound
    sound->PlaySound(GameSound::InGameMenuClosed, false);
	// Resume world sounds - these are initially paused when coming to this state (in the constructor)
	sound->UnpauseAllSounds();
	
    // Unpause the game
	this->display->GetModel()->UnsetPause(GameModel::PauseGame);

	// Go back to the in-game display state
    if (this->returnToDisplayState != NULL) {
	    this->display->SetCurrentState(returnToDisplayState);
    }
    else {
        this->display->SetCurrentState(new InGameDisplayState(this->display));
    }
}

// Make sure this is called if we aren't going back to the returnToDisplayState
// i.e., if we exit to main menu, desktop, etc.
void InGameMenuState::CleanUpReturnToDisplayState() {
    if (this->returnToDisplayState != NULL) {
        delete this->returnToDisplayState;
        this->returnToDisplayState = NULL;
    }
}

/**
 * Called whenever a key is pressed while in this state.
 */
void InGameMenuState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                    const GameControl::ActionMagnitude& magnitude) {
	
    UNUSED_PARAMETER(magnitude);                                    
    assert(this->topMenu != NULL);

	// If the pause button is hit again then just exit this menu back to the game...
	if (pressedButton == GameControl::PauseButtonAction) {
		this->ResumeTheGame();
	}
	else {
		// Tell the top-most menu about the key pressed event
		this->topMenu->ButtonPressed(pressedButton);
	}
}

/**
 * Called whenever a key is released while in this state.
 */
void InGameMenuState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	assert(this->topMenu != NULL);

	// Tell the top-most menu about the key released event
	this->topMenu->ButtonReleased(releasedButton);
}

void InGameMenuState::InitTopMenu() {
	assert(this->topMenu == NULL);
	assert(this->topMenuEventHandler == NULL);

	float textScaleFactor = this->display->GetTextScalingFactor();

	// Set up the handlers
	this->topMenuEventHandler    = new TopMenuEventHandler(this);
    this->difficultyEventHandler = new DifficultyEventHandler(this);

    this->difficultyVerifyHandler       = new DifficultyVerifyEventHandler(this);
    this->restartVerifyHandler          = new RestartVerifyEventHandler(this);
    this->exitGameVerifyHandler         = new ExitGameVerifyEventHandler(this);
    this->returnToMainMenuVerifyHandler = new ReturnToMainMenuVerifyEventHandler(this);

	// Setup the top menu attributes
	this->topMenu = new GameMenu();
	this->topMenu->SetAlignment(GameMenu::CenterJustified);
	this->topMenu->AddEventHandler(this->topMenuEventHandler);
	this->topMenu->SetColourScheme(InGameMenuState::MENU_ITEM_IDLE_COLOUR, InGameMenuState::MENU_ITEM_SEL_COLOUR, 
	    InGameMenuState::MENU_ITEM_ACTIVE_COLOUR, InGameMenuState::MENU_ITEM_GREYED_COLOUR);

	// Prepare some of the properties of the text labels in the top level menu...
	const float dropShadowAmtSm = 0.10f;
	const float dropShadowAmtLg = 0.10f;
	const Colour dropShadowColour = Colour(0.0f, 0.0f, 0.0f);

    static const char* VERIFY_EXIT_TO_MAIN_MENU_TEXT = "All progress in this level will be lost, exit to the main menu?";
    static const char* VERIFY_RESTART_TEXT           = "Are you sure you want to restart this level?";
    static const char* VERIFY_EXIT_TO_DESKTOP_TEXT   = "All progress in this level will be lost, are you sure you want to exit the game?";

    // Resume Item...
	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
        GameFontAssetsManager::Medium), "Resume");
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
        GameFontAssetsManager::Big),    "Resume");
    tempLabelSm.SetScale(textScaleFactor);
    tempLabelLg.SetScale(textScaleFactor);
	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);

	// Add items to the menu in their order (first to last)
	this->resumeItem = this->topMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
	
    // Restart Item...
	tempLabelSm.SetText("Restart Level");
	tempLabelLg.SetText("Restart Level");

	VerifyMenuItem* restartLevelItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	restartLevelItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	restartLevelItem->SetVerifyMenuText(VERIFY_RESTART_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    restartLevelItem->SetEventHandler(this->restartVerifyHandler);

    this->restartItem = this->topMenu->AddMenuItem(restartLevelItem);

    // We don't allow a difficulty item to be part of the menu if the player is in the tutorial
    if (this->returnToDisplayState->GetType() == DisplayState::InTutorialGame) {
        this->difficultyItem = GameMenu::NO_MENU_ITEM_INDEX;
    }
    else {

        // Add the difficulty item to the menu...
        static const int POPUP_WIDTH = 750;
        this->initialDifficultySelected = static_cast<int>(this->cfgOptions.GetDifficulty());
        
        tempLabelSm.SetText("");
	    tempLabelLg.SetText("");
	    VerifyMenuItem* verifyDifficultyMenuItem  = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		    GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	    verifyDifficultyMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	    verifyDifficultyMenuItem->SetVerifyMenuText("Changing the difficulty will restart the current level. Are you sure?", VERIFY_MENU_YES, VERIFY_MENU_NO);
        verifyDifficultyMenuItem->SetEventHandler(this->difficultyVerifyHandler);

        tempLabelSm.SetText("Difficulty");
	    tempLabelLg.SetText("Difficulty");
        std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();

        this->difficultyMenuItem = new SelectionListMenuItemWithVerify(tempLabelSm, tempLabelLg, difficultyOptions, verifyDifficultyMenuItem);
        this->difficultyMenuItem->SetSelectedItem(this->initialDifficultySelected);
        this->difficultyMenuItem->SetEventHandler(this->difficultyEventHandler);

        this->difficultyItem = this->topMenu->AddMenuItem(this->difficultyMenuItem); 
    }

	// The exit/return to main menu item has a verify menu...
	tempLabelSm.SetText("Return to Main Menu");
	tempLabelLg.SetText("Return to Main Menu");

	VerifyMenuItem* returnToMainMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	returnToMainMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	returnToMainMenuItem->SetVerifyMenuText(VERIFY_EXIT_TO_MAIN_MENU_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    returnToMainMenuItem->SetEventHandler(this->returnToMainMenuVerifyHandler);

	this->returnToMainItem = this->topMenu->AddMenuItem(returnToMainMenuItem);
	
	// The exit to desktop menu item has a verify menu...
	tempLabelSm.SetText("Exit Game");
	tempLabelLg.SetText("Exit Game");

	VerifyMenuItem* exitToDesktopMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	exitToDesktopMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	exitToDesktopMenuItem->SetVerifyMenuText(VERIFY_EXIT_TO_DESKTOP_TEXT, VERIFY_MENU_YES, VERIFY_MENU_NO);
    exitToDesktopMenuItem->SetEventHandler(this->exitGameVerifyHandler);

	this->exitToDesktopItem = this->topMenu->AddMenuItem(exitToDesktopMenuItem);

	this->topMenu->SetSelectedMenuItem(this->resumeItem, false);
}

// Top Menu Handler Functions ***********************************************************

void InGameMenuState::TopMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);

    // Play the sound effect assoicated with menu item changing/being highlighted by the user
    GameSound* sound = this->inGameMenuState->display->GetSound();
    sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false);
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {

    //GameSound* sound = this->inGameMenuState->display->GetSound();

	if (itemIndex == this->inGameMenuState->resumeItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ResumeGame;
	}
    else if (itemIndex == this->inGameMenuState->restartItem) {

    }
    else if (itemIndex == this->inGameMenuState->difficultyItem) {

    }
    else if (itemIndex == this->inGameMenuState->returnToMainItem) {

    }
    else if (itemIndex == this->inGameMenuState->exitToDesktopItem) {

    }
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
    UNUSED_PARAMETER(itemIndex);
}

void InGameMenuState::TopMenuEventHandler::EscMenu() {
	GameMenu* topMenu = this->inGameMenuState->topMenu;
	assert(topMenu != NULL);

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (topMenu->GetSelectedMenuItem() != this->inGameMenuState->resumeItem) {
		topMenu->SetSelectedMenuItem(this->inGameMenuState->resumeItem, true);
	}
	else {
		topMenu->ActivateSelectedMenuItem();
	}
}

// Verify Menu Event Handler ******************************************************
InGameMenuState::RestartVerifyEventHandler::RestartVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::RestartVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::RestartLevel;
}

InGameMenuState::ExitGameVerifyEventHandler::ExitGameVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ExitGameVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::ExitToDesktop;
}

InGameMenuState::ReturnToMainMenuVerifyEventHandler::ReturnToMainMenuVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ReturnToMainMenuVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->inGameMenuState->nextAction = InGameMenuState::ReturnToMainMenu;
}

InGameMenuState::DifficultyEventHandler::DifficultyEventHandler(InGameMenuState *inGameMenuState) : 
SelectionListEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

InGameMenuState::DifficultyVerifyEventHandler::DifficultyVerifyEventHandler(InGameMenuState* inGameMenuState) :
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::DifficultyVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();

    GameModel* gameModel = this->inGameMenuState->display->GetModel();
    assert(!gameModel->IsCurrentLevelTheTutorialLevel());

    // If the difficulty is actually being changed then will have to restart the current level...
    int currSelectionIdx = this->inGameMenuState->difficultyMenuItem->GetSelectedItemIndex();
    GameModel::Difficulty difficultyToSet = static_cast<GameModel::Difficulty>(currSelectionIdx);
    this->inGameMenuState->cfgOptions.SetDifficulty(difficultyToSet);
    gameModel->SetDifficulty(difficultyToSet);
    
    // Rewrite the configuration file to accomodate the change in difficulty
    ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->inGameMenuState->cfgOptions);

    // Flag to restart the level...
    this->inGameMenuState->nextAction = InGameMenuState::RestartLevel;
}

void InGameMenuState::DifficultyVerifyEventHandler::MenuItemCancelled() {
    VerifyMenuEventHandlerWithSound::MenuItemCancelled();

    GameModel* gameModel = this->inGameMenuState->display->GetModel();
    this->inGameMenuState->difficultyMenuItem->SetSelectedItem(static_cast<int>(gameModel->GetDifficulty()));
}