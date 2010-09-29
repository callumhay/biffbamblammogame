#include "GameController.h"

#include "GameView/GameDisplay.h"

#include "GameModel/GameModel.h"
#include "GameModel/GameItem.h"

#include "BlammoEngine/Camera.h"

#ifdef _DEBUG
// TODO: have seperated debug functionality
	float GameController::DEBUG_highlightThreshold	= 0.5f;
	float GameController::DEBUG_sceneIntensity			= 0.9f;
	float GameController::DEBUG_glowIntensity				= 0.7f;
	float GameController::DEBUG_highlightIntensity	= 0.65f;
#endif

GameController::GameController(GameModel* model, GameDisplay* display): model(model), display(display) {
	for (int i = 0; i < NUM_KEYS; i++) {
		this->keyPressed[i] = false;
	}
}

void GameController::KeyDown(SDLKey key, SDLMod modifier) {
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, true);
	this->display->KeyPressed(key, modifier);

	if (key == SDLK_SPACE) {
		this->model->ReleaseBall();
	}

#ifdef _DEBUG
	// Debug Item drops
	if (key == SDLK_a) {
		this->model->DropItem(GameItem::PaddleGrowItem);
	}
	else if (key == SDLK_b) {
		this->model->DropItem(GameItem::BallCamItem);
	}
	else if (key == SDLK_c) {
		this->model->DropItem(GameItem::PaddleCamItem);
	}
	else if (key == SDLK_d) {
		this->model->DropItem(GameItem::LaserBeamPaddleItem);
	}
	else if (key == SDLK_e) {
		GameDisplay::ToggleDrawDebugLightGeometry();
	}
	else if (key == SDLK_f) {
		this->model->DropItem(GameItem::UpsideDownItem);
	}
	else if (key == SDLK_g) {
		this->model->DropItem(GameItem::GhostBallItem);
	}
	else if (key == SDLK_h) {
		this->model->DropItem(GameItem::GravityBallItem);
	}
	else if (key == SDLK_i) {
		this->model->DropItem(GameItem::InvisiBallItem);
	}
	else if (key == SDLK_j) {
		this->model->DropItem(GameItem::CrazyBallItem);
	}
	else if (key == SDLK_k) {
		this->model->DropItem(GameItem::BallGrowItem);
	}
	else if (key == SDLK_l) {
		this->model->DropItem(GameItem::LaserBulletPaddleItem);
	}
	else if (key == SDLK_m) {
		this->model->DropItem(GameItem::BallShrinkItem);
	}
	else if (key == SDLK_n) {
		this->model->DropItem(GameItem::BallSafetyNetItem);
	}
	else if (key == SDLK_o) {
		this->model->DropItem(GameItem::ShieldPaddleItem);
	}
	else if (key == SDLK_p) {
		this->model->TogglePause(GameModel::PauseGame);
	}
	else if (key == SDLK_q) {
		this->model->DropItem(GameItem::BallSpeedUpItem);
	}
	else if (key == SDLK_r) {
		this->model->DropItem(GameItem::RocketPaddleItem);
	}
	else if (key == SDLK_s) {
		this->model->DropItem(GameItem::BallSlowDownItem);
	}
	// t
	else if (key == SDLK_u) {
		this->model->DropItem(GameItem::UberBallItem);
	}
	// v
	else if (key == SDLK_w) {
		GameDisplay::ToggleDrawDebugBounds();
	}
	else if (key == SDLK_x) {
		this->model->DropItem(GameItem::PoisonPaddleItem);
	}
	else if (key == SDLK_y) {
		this->model->DropItem(GameItem::StickyPaddleItem);
	}
	else if (key == SDLK_z) {
		this->model->DropItem(GameItem::PaddleShrinkItem);
	}
	else if (key == SDLK_0) {
		this->model->DropItem(GameItem::BlackoutItem);
	}
	else if (key == SDLK_1) {
		this->model->DropItem(GameItem::OneUpItem);
	}
	// 2
	else if (key == SDLK_3) {
		this->model->DropItem(GameItem::MultiBall3Item);
	}
	// 4
	else if (key == SDLK_5) {
		this->model->DropItem(GameItem::MultiBall5Item);
	}
	// 6
	// 7
	// 8
	// 9
	else if (key == SDLK_BACKSLASH) {
		this->model->DropItem(GameItem::RandomItem);
	}
	// ...

	else if (key == SDLK_SLASH) {
		GameDisplay::ToggleDetachCamera();
	}
	else if (key == SDLK_COMMA) {
		this->model->TogglePause(GameModel::PauseBall);
	}

#endif

}

void GameController::KeyUp(SDLKey key, SDLMod modifier) {
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, false);
	this->display->KeyReleased(key, modifier);
}

/*
 * Keeps keys in sync with the applications frames for smooth feedback.
 * This is called every frame.
 */
void GameController::Tick() {

	// Paddle controls (NOTE: the else is to make the feedback slicker)
	int paddleDir = this->model->AreControlsFlipped() ? 1 : -1;
	if (this->keyPressed[SDLK_LEFT]) {
		this->model->MovePaddle(paddleDir * this->model->GetPlayerPaddle()->GetSpeed());
	}
	else if (this->keyPressed[SDLK_RIGHT]) {
		this->model->MovePaddle(-paddleDir * this->model->GetPlayerPaddle()->GetSpeed());
	}

	// Debug movement controls
#ifdef _DEBUG
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