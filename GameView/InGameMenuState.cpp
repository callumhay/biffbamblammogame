/**
 * InGameMenuState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
topMenu(NULL), topMenuEventHandler(NULL), difficultyEventHandler(NULL), invertBallBoostHandler(NULL),
restartVerifyHandler(NULL), exitGameVerifyHandler(NULL), returnToMainMenuVerifyHandler(NULL),
returnToDisplayState(returnToDisplayState), initialDifficultySelected(-1),
difficultyRestartPopup(NULL) {

	// Pause all world sounds
	this->display->GetSound()->PauseAllSounds();

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
    delete this->invertBallBoostHandler;
    this->invertBallBoostHandler = NULL;

    delete this->restartVerifyHandler;
    this->restartVerifyHandler = NULL;
    delete this->exitGameVerifyHandler;
    this->exitGameVerifyHandler = NULL;
    delete this->returnToMainMenuVerifyHandler;
    this->returnToMainMenuVerifyHandler = NULL;

    if (this->difficultyRestartPopup != NULL) {
        delete this->difficultyRestartPopup;
        this->difficultyRestartPopup = NULL;
    }
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
	const Camera& camera = this->display->GetCamera();
	FBObj* lastFullscreenRendering = this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO();
	assert(lastFullscreenRendering != NULL);

	lastFullscreenRendering->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);
    renderPipeline.RenderHUD(0.0);

	// Render an overlay that dulls the background rendering...
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(),
        -1.0f, ColourRGBA(0.0f, 0.0f, 0.0f, 0.5f));
	
	// Draw the menus...
	glEnable(GL_DEPTH_TEST);
	this->topMenu->SetCenteredOnScreen(camera.GetWindowWidth(), camera.GetWindowHeight());
	this->topMenu->Draw(dT, camera.GetWindowWidth(), camera.GetWindowHeight());
	glPopAttrib();

    // Draw any extra popups/dialogs
    if (this->difficultyRestartPopup != NULL) {
        this->difficultyRestartPopup->Tick(dT);
        this->difficultyRestartPopup->Draw(camera);
    }
}

void InGameMenuState::ResumeTheGame() {
	// Resume world sounds - these are initially paused when coming to this state (in the constructor)
	this->display->GetSound()->UnpauseAllSounds();
	
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

    // Feed controls to the pop-up if it exists and is currently visible
    if (this->difficultyRestartPopup != NULL && this->difficultyRestartPopup->GetIsVisible()) {
        this->difficultyRestartPopup->ButtonPressed(pressedButton);
        return;
    }

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

    // If pop-up exists and is currently visible then controls all go to it
    if (this->difficultyRestartPopup != NULL && this->difficultyRestartPopup->GetIsVisible()) {
        return;
    }

	// Tell the top-most menu about the key released event
	this->topMenu->ButtonReleased(releasedButton);
}

void InGameMenuState::InitTopMenu() {
	assert(this->topMenu == NULL);
	assert(this->topMenuEventHandler == NULL);

	//const Camera& camera = this->display->GetCamera();
	const float textScaleFactor = this->display->GetTextScalingFactor();

	// Set up the handlers
	this->topMenuEventHandler    = new TopMenuEventHandler(this);
    this->difficultyEventHandler = new DifficultyEventHandler(this);
    this->invertBallBoostHandler = new InvertBallBoostEventHandler(this);

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
	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelSm.SetScale(textScaleFactor);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);
	tempLabelLg.SetScale(textScaleFactor);

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

    // Invert ball boost Item...
    //tempLabelSm.SetText("Invert Ball Boost");
    //tempLabelLg.SetText("Invert Ball Boost");
    //std::vector<std::string> invertOptions = ConfigOptions::GetOnOffItems();

    //this->invertBallBoostMenuItem = new SelectionListMenuItem(tempLabelSm, tempLabelLg, invertOptions);
    //this->invertBallBoostMenuItem->SetSelectedItem(this->cfgOptions.GetInvertBallBoost() ? 1 : 0);
    //this->invertBallBoostMenuItem->SetEventHandler(this->invertBallBoostHandler);

    //this->invertBallBoostItem = this->topMenu->AddMenuItem(this->invertBallBoostMenuItem);

    // We don't allow a difficulty item to be part of the menu if the player is in the tutorial
    if (this->returnToDisplayState->GetType() == DisplayState::InTutorialGame) {
        this->difficultyItem = GameMenu::NO_MENU_ITEM_INDEX;
    }
    else {
        // Add the difficulty item to the menu...
        tempLabelSm.SetText("Difficulty");
	    tempLabelLg.SetText("Difficulty");
        std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();

        static const int POPUP_WIDTH = 750;
        this->initialDifficultySelected = static_cast<int>(this->cfgOptions.GetDifficulty());
        
        std::vector<std::string> selectableOptions;
        selectableOptions.push_back(VERIFY_MENU_YES);
        selectableOptions.push_back(VERIFY_MENU_NO);

        DifficultyPopupHandler* popupHandler = new DifficultyPopupHandler(this);
        this->difficultyRestartPopup = new PopupTutorialHint(POPUP_WIDTH, popupHandler);
        DecoratorOverlayPane* difficultyRestartPopupPane = this->difficultyRestartPopup->GetPane();
        difficultyRestartPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
        difficultyRestartPopupPane->AddText("Changing the difficulty will restart the current level. Are you sure?", Colour(1,1,1), 1.0f);
        difficultyRestartPopupPane->SetSelectableOptions(selectableOptions, 1);

        this->difficultyMenuItem = new SelectionListMenuItem(tempLabelSm, tempLabelLg, difficultyOptions);
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
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->inGameMenuState->resumeItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ResumeGame;
	}
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {

    if (itemIndex == this->inGameMenuState->difficultyItem) {
        
        // If the difficulty is different then show the confirm dialog
        int currSelectionIdx = this->inGameMenuState->difficultyMenuItem->GetSelectedItemIndex();
        if (currSelectionIdx != this->inGameMenuState->initialDifficultySelected) {
            this->inGameMenuState->difficultyRestartPopup->Show(0.0, 0.5);
        }
    }
    //else if (itemIndex == this->inGameMenuState->invertBallBoostItem) {
    //    GameModel* gameModel = this->inGameMenuState->display->GetModel();

    //    bool isInverted = ConfigOptions::IsOnItemSelected(
    //        this->inGameMenuState->invertBallBoostMenuItem->GetSelectedItemIndex());
    //    gameModel->SetInvertBallBoostDir(isInverted);
    //    this->inGameMenuState->cfgOptions.SetInvertBallBoost(isInverted);
    //}
    else {
        return;
    }

    // A configuration option has changed - rewrite the configuration file to accomodate the change
    //ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->inGameMenuState->cfgOptions);
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemVerifiedEvent(int itemIndex) {
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
    SDL_Delay(500);
    this->inGameMenuState->nextAction = InGameMenuState::RestartLevel;
}

InGameMenuState::ExitGameVerifyEventHandler::ExitGameVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ExitGameVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(500);
    this->inGameMenuState->nextAction = InGameMenuState::ExitToDesktop;
}

InGameMenuState::ReturnToMainMenuVerifyEventHandler::ReturnToMainMenuVerifyEventHandler(InGameMenuState* inGameMenuState) : 
VerifyMenuEventHandlerWithSound(inGameMenuState->display->GetSound()),
inGameMenuState(inGameMenuState) {
}

void InGameMenuState::ReturnToMainMenuVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(500);
    this->inGameMenuState->nextAction = InGameMenuState::ReturnToMainMenu;
}

void InGameMenuState::DifficultyPopupHandler::OptionSelected(const std::string& optionText) {
    PaneHandler::OptionSelected(optionText);

    GameModel* gameModel = this->inGameMenuState->display->GetModel();
    assert(!gameModel->IsCurrentLevelTheTutorialLevel());

    if (optionText.compare(InGameMenuState::VERIFY_MENU_YES) == 0) {
        
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
    else {
        this->inGameMenuState->difficultyMenuItem->SetSelectedItem(this->inGameMenuState->initialDifficultySelected);
    }
}