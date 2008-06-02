#include "MainMenuDisplayState.h"
#include "InGameDisplayState.h"

#include "GameController.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "TextLabel.h"
#include "GameAssets.h"


#include "../Utils/Includes.h"

// TODO: this will eventually be replaced by 3 sprites
// for "Biff" "Bam" and "Blammo" respectively
const std::string MainMenuDisplayState::TITLE_TEXT = "BIFF! BAM!! BLAMMO!?!";

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
	}
	// Dispose of title label
	if (this->titleLabel != NULL) {
		delete this->titleLabel;
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
	this->titleLabel = new TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::ExplosionBoom, GameAssets::Huge), TITLE_TEXT);
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
	TextLabel2D tempLabel = TextLabel2D(this->display->GetAssets()->GetFont(GameAssets::AllPurpose, GameAssets::Big), NEW_GAME_MENUITEM);
	tempLabel.SetDropShadow(dropShadowColour, dropShadowAmt);
	this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(PLAY_LEVEL_MENUITEM);
	this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(OPTIONS_MENUITEM);
	this->menu->AddMenuItem(tempLabel);

	tempLabel.SetText(EXIT_MENUITEM);
	this->menu->AddMenuItem(tempLabel);

	this->menu->SetHighlightedMenuItem(0);
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame(double dT) {
	
	// Set up a perspective projection for the menu background
	// TODO

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
	this->menu->Draw();
	//this->menu->DebugDraw();

}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::KeyPressed(unsigned char key) {
	// We only care about keys that manipulate the choice in the menu
	switch(key) {
		case GLUT_KEY_DOWN:
			this->menu->DownAction();
			break;
		case GLUT_KEY_UP:
			this->menu->UpAction();
			break;
		case GameController::ENTER_CHAR:
			{
				// On enter we select the menu item that is currently highlighted...
				int selectedMenuItem = this->menu->GetHighlightedMenuItem();
				
				// TODO: animation or something for item and stuff...

				switch (selectedMenuItem) {
					case NEW_GAME_INDEX:
						debug_output("Selected " << NEW_GAME_MENUITEM << " from menu");
						this->display->SetCurrentState(new InGameDisplayState(this->display));

						break;
					case PLAY_LEVEL_INDEX:
						debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
						break;
					case OPTIONS_INDEX:
						debug_output("Selected " << OPTIONS_MENUITEM << " from menu");
						break;
					case EXIT_INDEX:
						// TODO: Put a dialogue for "Are you sure..."
						exit(0);
					default:
						assert(false);
						break;
				}
			}
			break;
		default:
			break;
	}
}
