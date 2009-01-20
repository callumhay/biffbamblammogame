#include "GameController.h"

#include "GameView/GameDisplay.h"

#include "GameModel/GameModel.h"
#include "GameModel/BallSpeedItem.h"
#include "GameModel/UberballItem.h"
#include "GameModel/InvisiBallItem.h"
#include "GameModel/GhostBallItem.h"

#include "BlammoEngine/BlammoEngine.h"
#include "BlammoEngine/Camera.h"

GameController::GameController(GameModel* model, GameDisplay* display): model(model), display(display) {
	for (int i = 0; i < NUM_KEYS; i++) {
		this->keyPressed[i] = false;
	}
}

void GameController::KeyDown(SDLKey key) {
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, true);
	this->display->KeyPressed(key);

#ifndef NDEBUG
	// Debug Item drops
	if (key == SDLK_s) {
		this->model->DropItem(BallSpeedItem::SLOW_BALL_ITEM_NAME);
	}
	else if (key == SDLK_q) {
		this->model->DropItem(BallSpeedItem::FAST_BALL_ITEM_NAME);
	}
	else if (key == SDLK_u) {
		this->model->DropItem(UberBallItem::UBER_BALL_ITEM_NAME);
	}
	else if (key == SDLK_i) {
		this->model->DropItem(InvisiBallItem::INVISI_BALL_ITEM_NAME);
	}
	else if (key == SDLK_g) {
		this->model->DropItem(GhostBallItem::GHOST_BALL_ITEM_NAME);
	}
	else if (key == SDLK_p) {
		this->model->TogglePauseGame();
	}
#endif

}

void GameController::KeyUp(SDLKey key) {
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, false);
}

/*
 * Keeps keys in sync with the applications frames for smooth feedback.
 * This is called every frame.
 */
void GameController::Tick() {

	// Paddle controls (NOTE: the else is to make the feedback slicker)
	if (this->keyPressed[SDLK_LEFT]) {
		this->model->MovePaddle(-this->model->GetPlayerPaddle()->GetSpeed());
	}
	else if (this->keyPressed[SDLK_RIGHT]) {
		this->model->MovePaddle(this->model->GetPlayerPaddle()->GetSpeed());
	}
	if (this->keyPressed[SDLK_SPACE]) {
		this->model->ReleaseBall();
	}

	// Debug movement controls
#ifndef NDEBUG
	if (this->keyPressed[SDLK_KP8]) {
		this->display->GetCamera().Move(Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed[SDLK_KP5]) {
		this->display->GetCamera().Move(-Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed[SDLK_KP4]) {
		this->display->GetCamera().Move(Camera::DEFAULT_LEFT_VEC);
	}
	else if (this->keyPressed[SDLK_KP6]) {
		this->display->GetCamera().Move(-Camera::DEFAULT_LEFT_VEC);
	}
	else if (this->keyPressed[SDLK_KP7]) {
		this->display->GetCamera().Rotate('y', 1.0f);
	}
	else if (this->keyPressed[SDLK_KP9]) {
		this->display->GetCamera().Rotate('y', -1.0f);
	}
	else if (this->keyPressed[SDLK_KP_DIVIDE]) {
		this->display->GetCamera().Rotate('x', 1.0f);
	}
	else if (this->keyPressed[SDLK_KP_MULTIPLY]) {
		this->display->GetCamera().Rotate('x', -1.0f);
	}
	else if (this->keyPressed[SDLK_KP1]) {
		this->display->GetCamera().Rotate('z', 1.0f);
	}
	else if (this->keyPressed[SDLK_KP3]) {
		this->display->GetCamera().Rotate('z', -1.0f);
	}
#endif
}