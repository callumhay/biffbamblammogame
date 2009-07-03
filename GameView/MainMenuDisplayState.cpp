/**
 * MainMenuDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MainMenuDisplayState.h"
#include "StartGameDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameMenuItem.h"
#include "GameAssets.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/FBObj.h"

#include "../GameController.h"

// TODO: this will eventually be replaced by 3 sprites
// for "Biff" "Bam" and "Blammo" respectively
const std::string MainMenuDisplayState::TITLE_TEXT = "Biff! Bam!! Blammo!?!";
const std::string MainMenuDisplayState::TITLE_BIFF_TEXT			= "Biff!";
const std::string MainMenuDisplayState::TITLE_BAM_TEXT			= "Bam!!";
const std::string MainMenuDisplayState::TITLE_BLAMMO_TEXT		= "Blammo!?!";

// Menu items
const std::string MainMenuDisplayState::NEW_GAME_MENUITEM		= "New Game";
const std::string MainMenuDisplayState::PLAY_LEVEL_MENUITEM	= "Play Level";
const std::string MainMenuDisplayState::OPTIONS_MENUITEM		= "Options";
const std::string MainMenuDisplayState::EXIT_MENUITEM				= "Exit Game";

const Colour MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR		= Colour(0.8f, 0.8f, 0.8f);;
const Colour MainMenuDisplayState::MENU_ITEM_SEL_COLOUR			= Colour(1, 1, 0);
const Colour MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR	= Colour(1, 1, 1);
const Colour MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR	= Colour(0.7f, 0.7f, 0.7f);

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : 
DisplayState(display), mainMenu(NULL), optionsSubMenu(NULL), titleLabel(NULL),
mainMenuEventHandler(NULL), optionsMenuEventHandler(NULL), changeToPlayGameState(false),
menuFBO(NULL), bloomEffect(NULL) {
	this->mainMenuEventHandler		= new MainMenuEventHandler(this);
	this->optionsMenuEventHandler = new OptionsSubMenuEventHandler(this);
	
	this->InitializeMainMenu();

	// Setup the fade-in animation
	this->fadeAnimation.SetLerp(0.0, 3.0, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

	this->menuFBO			= new FBObj(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
}

MainMenuDisplayState::~MainMenuDisplayState() {
	// Dispose of the menu object
	if (this->mainMenu != NULL) {
		delete this->mainMenu;
		this->mainMenu = NULL;
	}

	if (this->optionsSubMenu != NULL) {
		delete this->optionsSubMenu;
		this->optionsSubMenu = NULL;
	}

	// Dispose of title label
	if (this->titleLabel != NULL) {
		delete this->titleLabel;
		this->titleLabel = NULL;
	}

	delete this->mainMenuEventHandler;
	this->mainMenuEventHandler = NULL;
	delete this->optionsMenuEventHandler;
	this->optionsMenuEventHandler = NULL;

	delete this->menuFBO;
	this->menuFBO = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;
}

/**
 * Used as a private helper method to initialize the menu and other related display stuffs.
 */
void MainMenuDisplayState::InitializeMainMenu() {
	unsigned int titleHeight = this->display->GetDisplayHeight() - TITLE_Y_INDENT;
	const float dropShadowAmt = 0.08f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	// Setup the title
	this->titleLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), TITLE_TEXT);
	this->titleLabel->SetTopLeftCorner(Point2D(MENU_X_INDENT, titleHeight));
	this->titleLabel->SetDropShadow(dropShadowColour, dropShadowAmt);
	this->titleLabel->SetColour(Colour(1, 0.8, 0));

	// Setup the main menu attributes
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, titleHeight - this->titleLabel->GetHeight() - MENU_Y_INDENT);

	// Setup the main menu (top-most menu)
	this->mainMenu = new GameMenu(menuTopLeftCorner);
	this->mainMenu->AddEventHandler(this->mainMenuEventHandler);
	this->mainMenu->SetPaddingBetweenMenuItems(MENU_ITEM_PADDING);
	this->mainMenu->SetColourScheme(MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
																	MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Setup submenues
	this->InitializeOptionsSubMenu();

	// Add items to the menu in their order (first to last)
	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),  NEW_GAME_MENUITEM);
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), NEW_GAME_MENUITEM);

	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmt);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmt);

	this->newGameMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	tempLabelSm.SetText(PLAY_LEVEL_MENUITEM);
	tempLabelLg.SetText(PLAY_LEVEL_MENUITEM);
	this->playLevelMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	// Setup the options item in the main menu and its submenu
	tempLabelSm.SetText(OPTIONS_MENUITEM);
	tempLabelLg.SetText(OPTIONS_MENUITEM);
	this->optionsMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, this->optionsSubMenu);
	
	tempLabelSm.SetText(EXIT_MENUITEM);
	tempLabelLg.SetText(EXIT_MENUITEM);
	this->exitGameMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	this->mainMenu->SetSelectedMenuItem(this->newGameMenuItemIndex);
	debug_opengl_state();
}

/**
 * Private helper function for initializing the options sub menu.
 */
void MainMenuDisplayState::InitializeOptionsSubMenu() {
	// Setup the options menu attributes
	const float dropShadowAmt = 0.08f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	TextLabel2D subMenuLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),  "Fullscreen");
	TextLabel2D subMenuLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "Fullscreen");
	subMenuLabelSm.SetDropShadow(dropShadowColour, dropShadowAmt);
	subMenuLabelLg.SetDropShadow(dropShadowColour, dropShadowAmt);

	// Options Menu
	this->optionsSubMenu = new GameSubMenu();
	this->optionsSubMenu->AddEventHandler(this->optionsMenuEventHandler);
	this->optionsSubMenu->SetPaddingBetweenMenuItems(MENU_ITEM_PADDING);
	this->optionsSubMenu->SetColourScheme(MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
																				MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Add the toggle fullscreen item
	//subMenuLabelSm.SetText("TODO: Fullscreen:");
	//subMenuLabelLg.SetText("TODO: Fullscreen:");
	std::vector<std::string> fullscreenOptions;
	fullscreenOptions.reserve(2);
	fullscreenOptions.push_back("Off");
	fullscreenOptions.push_back("On");
	SelectionListMenuItem* fullscreenMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, fullscreenOptions);
	//fullscreenMenuItem->SetCurrSelection(/*TODO*/);
	this->optionsFullscreenIndex = this->optionsSubMenu->AddMenuItem(fullscreenMenuItem);

	// Add the vertical sync toggle item
	subMenuLabelSm.SetText("Vertical Sync");
	subMenuLabelLg.SetText("Vertical Sync");
	std::vector<std::string> vSyncOptions;
	vSyncOptions.reserve(2);
	vSyncOptions.push_back("Off");
	vSyncOptions.push_back("On");
	SelectionListMenuItem* vSyncMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, vSyncOptions);
	//vSyncMenuItem->SetCurrSelection(/*TODO*/);
	this->optionsVSyncIndex = this->optionsSubMenu->AddMenuItem(vSyncMenuItem);

	// Add the resolution selection item
	subMenuLabelSm.SetText("Resolution");
	subMenuLabelLg.SetText("Resolution");
	
	std::vector<std::string> resolutionOptions;
	resolutionOptions.reserve(12);
	resolutionOptions.push_back("800x600");
	resolutionOptions.push_back("1024x768");
	resolutionOptions.push_back("1152x864");
	resolutionOptions.push_back("1280x800");
	resolutionOptions.push_back("1280x960");
	resolutionOptions.push_back("1280x1024");
	resolutionOptions.push_back("1360x768");
	resolutionOptions.push_back("1440x900");
	resolutionOptions.push_back("1600x1200");
	resolutionOptions.push_back("1680x1050");
	resolutionOptions.push_back("1920x1080");
	resolutionOptions.push_back("1920x1200");
	SelectionListMenuItem* resolutionMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, resolutionOptions);
	//resolutionMenuItem->SetCurrSelection(/*TODO*/);
	this->optionsResolutionIndex = this->optionsSubMenu->AddMenuItem(resolutionMenuItem);

	// Add an option for getting out of the menu
	subMenuLabelSm.SetText("Back");
	subMenuLabelLg.SetText("Back");
	this->optionsBackIndex = this->optionsSubMenu->AddMenuItem(subMenuLabelSm, subMenuLabelLg, NULL);

	// Set the selected index to be the first item in the menu
	this->optionsSubMenu->SetSelectedMenuItem(this->optionsFullscreenIndex);
}

void MainMenuDisplayState::SetupBloomEffect() {
	if (this->bloomEffect != NULL) {
		delete this->bloomEffect;
	}

	// Create the new bloom effect and set its parameters appropriately
	this->bloomEffect = new CgFxBloom(this->menuFBO);

	this->bloomEffect->SetHighlightThreshold(0.4f);
	this->bloomEffect->SetSceneIntensity(0.6f);
	this->bloomEffect->SetGlowIntensity(0.3f);
	this->bloomEffect->SetHighlightIntensity(0.1f);
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame(double dT) {
	if (this->changeToPlayGameState) {
		this->display->SetCurrentState(new StartGameDisplayState(this->display));
		return;
	}
	
	const int DISPLAY_WIDTH		= this->display->GetDisplayWidth();
	const int DISPLAY_HEIGHT	= this->display->GetDisplayHeight();

	this->menuFBO->BindFBObj();

	// Render the background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Render the title
	unsigned int titleHeight = DISPLAY_HEIGHT - TITLE_Y_INDENT;
	this->titleLabel->SetTopLeftCorner(Point2D(MENU_X_INDENT, titleHeight));
	this->titleLabel->Draw();

	// Render the menu
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, titleHeight - this->titleLabel->GetHeight() - MENU_Y_INDENT);
	this->mainMenu->SetTopLeftCorner(menuTopLeftCorner);
	this->mainMenu->Draw(dT);

	// Fade-in/out overlay
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1.0f, 
																									 ColourRGBA(1, 1, 1, this->fadeAnimation.GetInterpolantValue()));
	glDisable(GL_BLEND);
	
	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu screen and draw it
	this->bloomEffect->Draw(DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
	this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	debug_opengl_state();

	this->fadeAnimation.Tick(dT);
}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::KeyPressed(SDLKey key) {
	// Tell the main menu about the key pressed event
	this->mainMenu->KeyPressed(key);
}

void MainMenuDisplayState::DisplaySizeChanged(int width, int height) {
	delete this->menuFBO;
	this->menuFBO = new FBObj(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
}

/**
 * Handle activation events for items in the main menu.
 */
void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->mainMenuState->newGameMenuItemIndex) {
		debug_output("Selected " << NEW_GAME_MENUITEM << " from menu");
		this->mainMenuState->changeToPlayGameState = true;
	}
	else if (itemIndex == this->mainMenuState->playLevelMenuItemIndex) {
		debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
	}
	else if (itemIndex == this->mainMenuState->optionsMenuItemIndex) {
		debug_output("Selected " << OPTIONS_MENUITEM << " from menu");
	}
	else if (itemIndex == this->mainMenuState->exitGameMenuItemIndex) {
		// TODO: Put a dialogue for "Are you sure..."
		this->mainMenuState->display->QuitGame();
	}
	else {
		assert(false);
	}
}

void MainMenuDisplayState::MainMenuEventHandler::EscMenu() {
	GameMenu* mainMenu = this->mainMenuState->mainMenu;

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (mainMenu->GetSelectedMenuItem() != this->mainMenuState->exitGameMenuItemIndex) {
		mainMenu->SetSelectedMenuItem(this->mainMenuState->exitGameMenuItemIndex);
	}
	else {
		this->mainMenuState->mainMenu->ActivateSelectedMenuItem();
	}
}

/**
 * Handle activation events for items in the options menu.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	// TODO... going to need another parameter for certain options...

	if (itemIndex == this->mainMenuState->optionsBackIndex) {
		// Go back to the main menu (deactivate any previously activated menu items)
		this->mainMenuState->optionsSubMenu->DeactivateSelectedMenuItem();
		this->mainMenuState->optionsSubMenu->SetSelectedMenuItem(0);

		this->mainMenuState->mainMenu->DeactivateSelectedMenuItem();
	}

}

void MainMenuDisplayState::OptionsSubMenuEventHandler::EscMenu() {
	GameSubMenu* optionsSubMenu = this->mainMenuState->optionsSubMenu;
	
	// Cause the menu to go back, if back isn't selected the select it...
	if (optionsSubMenu->GetSelectedMenuItem() != this->mainMenuState->optionsBackIndex) {
		optionsSubMenu->SetSelectedMenuItem(this->mainMenuState->optionsBackIndex);
	}
	else {
		this->mainMenuState->optionsSubMenu->ActivateSelectedMenuItem();
	}
}