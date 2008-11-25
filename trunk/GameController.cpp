#include "GameController.h"

#include "GameView/GameDisplay.h"
#include "GameModel/GameModel.h"

#include "BlammoEngine/BlammoEngine.h"

/*
GLUT_KEY_F1		F1 function key
GLUT_KEY_F2		F2 function key
GLUT_KEY_F3		F3 function key
GLUT_KEY_F4		F4 function key
GLUT_KEY_F5		F5 function key
GLUT_KEY_F6		F6 function key
GLUT_KEY_F7		F7 function key
GLUT_KEY_F8		F8 function key
GLUT_KEY_F9		F9 function key
GLUT_KEY_F10		F10 function key
GLUT_KEY_F11		F11 function key
GLUT_KEY_F12		F12 function key
GLUT_KEY_LEFT		Left function key
GLUT_KEY_RIGHT		Up function key
GLUT_KEY_UP		Right function key
GLUT_KEY_DOWN		Down function key
GLUT_KEY_PAGE_UP	Page Up function key
GLUT_KEY_PAGE_DOWN	Page Down function key
GLUT_KEY_HOME		Home function key
GLUT_KEY_END		End function key
GLUT_KEY_INSERT		Insert function key

*/

GameController::GameController(GameModel* model, GameDisplay* display): model(model), display(display) {
	for (int i = 0; i < NUM_KEYS; i++) {
		this->keyPressed[i] = false;
	}
}

/*
 * Process keys pressed within the context of the game.
 */
void GameController::ProcessNormalKeys(unsigned char key, int x, int y) {
	//debug_output("Key Pressed: " << "\"" << key << "\""); 

	switch(key) {
		case SPACE_BAR_CHAR:
			this->model->ReleaseBall();
			break;
		case ESC_CHAR:
			exit(0);
			break;
		default:
			break;
	}

	this->SetKeyPress(key, true);
	this->display->KeyPressed(key);
}

void GameController::ProcessNormalKeysUp(unsigned char key, int x, int y) {
	//debug_output("Key Released: " << "\"" << key << "\""); 
	/*
	switch(key) {
		case SPACE_BAR_CHAR:
			break;
		case ESC_CHAR:
			break;
		default:
			break;
	}
	*/
	this->SetKeyPress(key, false);
	
#ifndef NDEBUG
	// Debug Item drops
	if (key == 's') {
		this->model->DropSlowBallItem();
	}
	else if (key == 'q') {
		this->model->DropFastBallItem();
	}
	else if (key == 'u') {
		this->model->DropUberBallItem();
	}
	else if (key == 'i') {
		this->model->DropInvisiBallItem();
	}
	else if (key == 'p') {
		this->model->TogglePauseGame();
	}
#endif
}

void GameController::ProcessSpecialKeys(int key, int x, int y) {
	//debug_output("Special Key Pressed: " << "\"" << key << "\""); 
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, true);
	this->display->KeyPressed(key);
}

void GameController::ProcessSpecialKeysUp(int key, int x, int y) {
	//debug_output("Special Key Released: " << "\"" << key << "\"");
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, false);

}

void GameController::ProcessMouse(int button, int state, int x, int y) {
	// button can be GLUT_LEFT_BUTTON or GLUT_MIDDLE_BUTTON or GLUT_RIGHT_BUTTON
	// state can be either GLUT_DOWN or GLUT_UP
}

void GameController::ProcessActiveMouseMotion(int x,int y) {
}

void GameController::ProcessPassiveMouseMotion(int x, int y){
}

/*
 * Keeps keys in sync with the applications frames for smooth feedback.
 * This is called every frame.
 */
void GameController::Tick() {
	// Paddle controls (NOTE: the else is to make the feedback slicker)
	if (this->keyPressed[GLUT_KEY_LEFT]) {
		this->model->MovePaddle(-this->model->GetPlayerPaddle()->GetSpeed());
	}
	else if (this->keyPressed[GLUT_KEY_RIGHT]) {
		this->model->MovePaddle(this->model->GetPlayerPaddle()->GetSpeed());
	}

	// Debug movement controls
#ifndef NDEBUG
	if (this->keyPressed['8']) {
		this->display->GetCamera().Move(Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed['5']) {
		this->display->GetCamera().Move(-Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed['4']) {
		this->display->GetCamera().Move(Camera::DEFAULT_LEFT_VEC);
	}
	else if (this->keyPressed['6']) {
		this->display->GetCamera().Move(-Camera::DEFAULT_LEFT_VEC);
	}
	else if (this->keyPressed['7']) {
		this->display->GetCamera().Rotate('y', 1.0f);
	}
	else if (this->keyPressed['9']) {
		this->display->GetCamera().Rotate('y', -1.0f);
	}
#endif
}