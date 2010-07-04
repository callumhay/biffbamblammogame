#include "InGameMenuState.h"
#include "InGameDisplayState.h"
#include "MainMenuDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"

#include "../GameModel/GameModel.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../BlammoEngine/Camera.h"

const Colour InGameMenuState::MENU_ITEM_IDLE_COLOUR		= Colour(1.0f, 1.0f, 1.0f);
const Colour InGameMenuState::MENU_ITEM_SEL_COLOUR		= Colour(1, 0.65f, 0);
const Colour InGameMenuState::MENU_ITEM_ACTIVE_COLOUR	= Colour(0.49f, 0.98f, 1.0f);
const Colour InGameMenuState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);

InGameMenuState::InGameMenuState(GameDisplay* display) : DisplayState(display), nextAction(InGameMenuState::Nothing),
topMenu(NULL), topMenuEventHandler(NULL), verifyMenuEventHandler(NULL) {

	// Pause all world sounds
	this->display->GetAssets()->GetSoundAssets()->PauseWorldSounds();	
	// Pause the game itself
	this->display->GetModel()->SetPause(GameModel::PauseGame);


	this->InitTopMenu();
}

InGameMenuState::~InGameMenuState() {
	delete this->topMenu;
	this->topMenu = NULL;
	delete this->topMenuEventHandler;
	this->topMenuEventHandler = NULL;
	delete this->verifyMenuEventHandler;
	this->verifyMenuEventHandler = NULL;
}

/**
 * Render a frame - this will render the last frame from the game with an overlay of the
 * in-game menu in its current state according to interaction with the user.
 */
void InGameMenuState::RenderFrame(double dT) {
	switch (this->nextAction) {

		case InGameMenuState::ResumeGame:
			// Resume world sounds - these are initially paused when coming to this state (in the constructor)
			this->display->GetAssets()->GetSoundAssets()->UnpauseWorldSounds();
			// Unpause the game
			this->display->GetModel()->UnsetPause(GameModel::PauseGame);

			// Go back to the in-game display state
			this->display->SetCurrentState(new InGameDisplayState(this->display));
			return;

		case InGameMenuState::ReturnToMainMenu:
			// Clean up any misc. visual effects
			this->display->GetAssets()->DeactivateMiscEffects();
			// Kill all sounds
			this->display->GetAssets()->GetSoundAssets()->StopAllSounds();
			// Go back to the main menu state
			this->display->SetCurrentState(new MainMenuDisplayState(this->display));
			return;

		case InGameMenuState::ExitToDesktop:
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

	// Render an overlay that dulls the background rendering...
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), -1.0f, ColourRGBA(0.0f, 0.0f, 0.0f, 0.5f));
	
	// Draw the menus...
	glEnable(GL_DEPTH_TEST);
	this->topMenu->SetCenteredOnScreen(camera.GetWindowWidth(), camera.GetWindowHeight());
	this->topMenu->Draw(dT, camera.GetWindowWidth(), camera.GetWindowHeight());
	glPopAttrib();
}

/**
 * Called whenever a key is pressed while in this state.
 */
void InGameMenuState::KeyPressed(SDLKey key, SDLMod modifier) {
	assert(this->topMenu != NULL);
	// Tell the top-most menu about the key pressed event
	this->topMenu->KeyPressed(key, modifier);
}

/**
 * Called whenever a key is released while in this state.
 */
void InGameMenuState::KeyReleased(SDLKey key, SDLMod modifier) {
	assert(this->topMenu != NULL);
	// Tell the top-most menu about the key released event
	this->topMenu->KeyReleased(key, modifier);
}

void InGameMenuState::DisplaySizeChanged(int width, int height) {
}


void InGameMenuState::InitTopMenu() {
	assert(this->topMenu == NULL);
	assert(this->topMenuEventHandler == NULL);

	const Camera& camera = this->display->GetCamera();

	// Set up the handlers
	this->topMenuEventHandler			= new TopMenuEventHandler(this);
	this->verifyMenuEventHandler	= new VerifyMenuEventHandler(this);

	// Setup the top menu attributes
	this->topMenu = new GameMenu();
	this->topMenu->SetAlignment(GameMenu::CenterJustified);
	this->topMenu->AddEventHandler(this->topMenuEventHandler);
	this->topMenu->SetColourScheme(InGameMenuState::MENU_ITEM_IDLE_COLOUR, InGameMenuState::MENU_ITEM_SEL_COLOUR, 
																 InGameMenuState::MENU_ITEM_ACTIVE_COLOUR, InGameMenuState::MENU_ITEM_GREYED_COLOUR);

	// Prepare some of the properties of the text labels in the top level menu...
	const float dropShadowAmtSm = 0.10f;
	const float dropShadowAmtLg = 0.10f;
	const Colour dropShadowColour = Colour(0.62f, 0.72f, 0.80f);

	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),  "Resume");
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),     "Resume");
	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);	

	// Add items to the menu in their order (first to last)
	this->resumeItem = this->topMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
	
	// The exit/return to main menu item has a verify menu...
	tempLabelSm.SetText("Return to Main Menu");
	tempLabelLg.SetText("Return to Main Menu");

	VerifyMenuItem* returnToMainMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	returnToMainMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	returnToMainMenuItem->SetVerifyMenuText("All unsaved progress will be lost, exit to the main menu?", "Yes!", "NOoo!");
	returnToMainMenuItem->SetEventHandler(this->verifyMenuEventHandler);

	this->returnToMainItem = this->topMenu->AddMenuItem(returnToMainMenuItem);
	
	// The exit to desktop menu item has a verify menu...
	tempLabelSm.SetText("Exit to Desktop");
	tempLabelLg.SetText("Exit to Desktop");

	VerifyMenuItem* exitToDesktopMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	exitToDesktopMenuItem->SetVerifyMenuColours(Colour(1,1,1), InGameMenuState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	exitToDesktopMenuItem->SetVerifyMenuText("All unsaved progress will be lost, exit to the desktop?", "Yes!", "NOoo!");
	exitToDesktopMenuItem->SetEventHandler(this->verifyMenuEventHandler);

	this->exitToDesktopItem = this->topMenu->AddMenuItem(exitToDesktopMenuItem);

	this->topMenu->SetSelectedMenuItem(this->resumeItem);
}

// Top Menu Handler Functions ***********************************************************

void InGameMenuState::TopMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
}

void InGameMenuState::TopMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->inGameMenuState->resumeItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ResumeGame;
	}


}

void InGameMenuState::TopMenuEventHandler::GameMenuItemVerifiedEvent(int itemIndex) {
	if (itemIndex == this->inGameMenuState->returnToMainItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ReturnToMainMenu;
	}	
	else if (itemIndex == this->inGameMenuState->exitToDesktopItem) {
		this->inGameMenuState->nextAction = InGameMenuState::ExitToDesktop;
	}
}

void InGameMenuState::TopMenuEventHandler::EscMenu() {
	GameMenu* topMenu = this->inGameMenuState->topMenu;
	assert(topMenu != NULL);

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (topMenu->GetSelectedMenuItem() != this->inGameMenuState->resumeItem) {
		topMenu->SetSelectedMenuItem(this->inGameMenuState->resumeItem);
	}
	else {
		topMenu->ActivateSelectedMenuItem();
	}
}

// Verify Menu Event Handler ******************************************************
void InGameMenuState::VerifyMenuEventHandler::MenuItemScrolled() {
}

void InGameMenuState::VerifyMenuEventHandler::MenuItemEnteredAndSet() {
}

void InGameMenuState::VerifyMenuEventHandler::MenuItemCancelled() {
}