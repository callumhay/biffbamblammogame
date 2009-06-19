#include "MainMenuDisplayState.h"
#include "StartGameDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameAssets.h"

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

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : DisplayState(display), menu(NULL), titleLabel(NULL) {
	this->InitializeMenu();
}

MainMenuDisplayState::~MainMenuDisplayState() {
	// Dispose of the menu object
	if (this->menu != NULL) {
		delete this->menu;
		this->menu = NULL;
	}
	// Dispose of title label
	if (this->titleLabel != NULL) {
		delete this->titleLabel;
		this->titleLabel = NULL;
	}
}

/**
 * Used as a private helper method to initialize the menu and other related display stuffs.
 */
void MainMenuDisplayState::InitializeMenu() {
	unsigned int titleHeight = this->display->GetDisplayHeight() - TITLE_Y_INDENT;
	float dropShadowAmt = 0.08f;
	Colour dropShadowColour = Colour(0, 0, 0);

	// Setup the title
	this->titleLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), TITLE_TEXT);
	this->titleLabel->SetTopLeftCorner(Point2D(MENU_X_INDENT, titleHeight));
	this->titleLabel->SetDropShadow(dropShadowColour, dropShadowAmt);
	this->titleLabel->SetColour(Colour(1, 0.8, 0));

	// Setup the main menu attributes
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, titleHeight - this->titleLabel->GetHeight() - MENU_Y_INDENT);
	
	Colour idleColour(0.8f, 0.8f, 0.8f);
	Colour highlightColour(1, 1, 0);
	Colour selectColour(1, 0, 0);

	this->menu = new GameMenu(menuTopLeftCorner);
	this->menu->SetPaddingBetweenMenuItems(MENU_ITEM_PADDING);
	this->menu->SetColourScheme(idleColour, highlightColour, selectColour);

	// Add items to the menu in their order (first to last)
	TextLabel2D tempLabel = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), NEW_GAME_MENUITEM);
	tempLabel.SetDropShadow(dropShadowColour, dropShadowAmt);
	this->newGameMenuItemIndex = this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(PLAY_LEVEL_MENUITEM);
	this->playLevelMenuItemIndex = this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(OPTIONS_MENUITEM);
	this->optionsMenuItemIndex = this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(EXIT_MENUITEM);
	this->exitGameMenuItemIndex = this->menu->AddMenuItem(tempLabel);

	this->menu->SetSelectedMenuItem(this->newGameMenuItemIndex);

	debug_opengl_state();
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame(double dT) {
	// Render the background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// TODO

	// Render the title
	unsigned int titleHeight = this->display->GetDisplayHeight() - TITLE_Y_INDENT;
	this->titleLabel->SetTopLeftCorner(Point2D(MENU_X_INDENT, titleHeight));
	this->titleLabel->Draw();

	// Render the menu
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, titleHeight - this->titleLabel->GetHeight() - MENU_Y_INDENT);
	this->menu->SetTopLeftCorner(menuTopLeftCorner);
	this->menu->Draw(dT);
	//this->menu->DebugDraw();

	debug_opengl_state();
}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::KeyPressed(SDLKey key) {
	// We only care about keys that manipulate the choice in the menu
	switch(key) {
		case SDLK_DOWN:
			this->menu->DownAction();
			break;
		case SDLK_UP:
			this->menu->UpAction();
			break;
		case SDLK_RETURN: {
			// On enter we select the menu item that is currently highlighted...
			int selectedMenuItem = this->menu->GetSelectedMenuItem();
			
			// TODO: animation or something for item and stuff...

			if (selectedMenuItem == this->newGameMenuItemIndex) {
				debug_output("Selected " << NEW_GAME_MENUITEM << " from menu");
				this->display->SetCurrentState(new StartGameDisplayState(this->display));
			}
			else if (selectedMenuItem == this->playLevelMenuItemIndex) {
				debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
			}
			else if (selectedMenuItem == this->optionsMenuItemIndex) {
				debug_output("Selected " << OPTIONS_MENUITEM << " from menu");
			}
			else if (selectedMenuItem == this->exitGameMenuItemIndex) {
				// TODO: Put a dialogue for "Are you sure..."
				this->display->QuitGame();
			}
			else {
				assert(false);
			}
		}
	}
}

void MainMenuDisplayState::DisplaySizeChanged(int width, int height) {
}