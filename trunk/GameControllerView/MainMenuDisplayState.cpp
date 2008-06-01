#include "MainMenuDisplayState.h"
#include "GameController.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameAssets.h"

#include "../Utils/Includes.h"

Point2D MainMenuDisplayState::MENU_TOPLEFT_CORNER = Point2D(10, 600);

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : DisplayState(display), menu(NULL) {
	this->InitializeMenu();
}

MainMenuDisplayState::~MainMenuDisplayState() {
	// Dispose of the menu object
	if (this->menu != NULL) {
		delete this->menu;
	}
}

void MainMenuDisplayState::InitializeMenu() {
	// Set up the main menu attributes
	this->menu = new GameMenu(MENU_TOPLEFT_CORNER);
	this->menu->SetMenuItemPadding(MENU_ITEM_PADDING);

	// Add items to the menu in their order (first to last)
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::BloodCrunch, GameAssets::Small), "Test Menu Item 1a"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::BloodCrunch, GameAssets::Medium), "Test Menu Item 1b"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::BloodCrunch, GameAssets::Big), "Test Menu Item 1c"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::ExplosionBoom, GameAssets::Small), "Test Menu Item 2a"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::ExplosionBoom, GameAssets::Medium), "Test Menu Item 2b"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::ExplosionBoom, GameAssets::Big), "Test Menu Item 2c"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::GunBlam, GameAssets::Small), "Test Menu Item 3a"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::GunBlam, GameAssets::Medium), "Test Menu Item 3b"));
	this->menu->AddMenuItem(GameMenuItem(this->display->GetAssets()->GetFont(GameAssets::GunBlam, GameAssets::Big), "Test Menu Item 3c"));

}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set up a perspective projection for the menu background

	// Render the background


	// Setup an orthogonal projection for the menu
	//glViewport(0, 0, this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0, this->display->GetDisplayWidth(), 0, this->display->GetDisplayHeight());

	// Render the menu
	this->menu->Draw();
	this->menu->DebugDraw();
}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::KeyPressed(unsigned char key) {
	// We only care about keys that manipulate the choice in the menu
	switch(key) {
		case GLUT_KEY_DOWN:

			break;
		case GLUT_KEY_UP:

			break;
		case GameController::ENTER_CHAR:

			break;
		default:
			break;
	}
}
