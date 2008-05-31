#include "MainMenuDisplayState.h"
#include "GameController.h"
#include "GameDisplay.h"

#include "../Utils/Includes.h"

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : DisplayState(display) {
}

MainMenuDisplayState::~MainMenuDisplayState() {
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Set up a perspective projection for the menu background

	// Render the background


	// Setup an orthogonal projection for the menu
	glViewport(0, 0, this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, this->display->GetDisplayWidth(), 0, this->display->GetDisplayHeight());

	// Render the menu

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
