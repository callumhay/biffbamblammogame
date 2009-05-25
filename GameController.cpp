#include "GameController.h"

#include "GameView/GameDisplay.h"

#include "GameModel/GameModel.h"
#include "GameModel/BallSpeedItem.h"
#include "GameModel/UberballItem.h"
#include "GameModel/InvisiBallItem.h"
#include "GameModel/GhostBallItem.h"
#include "GameModel/LaserPaddleItem.h"
#include "GameModel/MultiBallItem.h"
#include "GameModel/PaddleSizeItem.h"
#include "GameModel/BallSizeItem.h"
#include "GameModel/BlackoutItem.h"
#include "GameModel/UpsideDownItem.h"
#include "GameModel/BallSafetyNetItem.h"
#include "GameModel/OneUpItem.h"
#include "GameModel/PoisonPaddleItem.h"

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

void GameController::KeyDown(SDLKey key) {
	if (key < 0 || key > NUM_KEYS) { return; }
	this->SetKeyPress(key, true);
	this->display->KeyPressed(key);

	if (key == SDLK_SPACE) {
		this->model->ReleaseBall();
	}

#ifdef _DEBUG
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
	else if (key == SDLK_l) {
		this->model->DropItem(LaserPaddleItem::LASER_PADDLE_ITEM_NAME);
	}
	else if (key == SDLK_3) {
		this->model->DropItem(MultiBallItem::MULTI3_BALL_ITEM_NAME);
	}
	else if (key == SDLK_5) {
		this->model->DropItem(MultiBallItem::MULTI5_BALL_ITEM_NAME);
	}
	else if (key == SDLK_a) {
		this->model->DropItem(PaddleSizeItem::PADDLE_GROW_ITEM_NAME);
	}
	else if (key == SDLK_z) {
		this->model->DropItem(PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME);
	}
	else if (key == SDLK_k) {
		this->model->DropItem(BallSizeItem::BALL_GROW_ITEM_NAME);
	}
	else if (key == SDLK_m) {
		this->model->DropItem(BallSizeItem::BALL_SHRINK_ITEM_NAME);
	}
	else if (key == SDLK_0) {
		this->model->DropItem(BlackoutItem::BLACKOUT_ITEM_NAME);
	}
	else if (key == SDLK_f) {
		this->model->DropItem(UpsideDownItem::UPSIDEDOWN_ITEM_NAME);
	}
	else if (key == SDLK_n) {
		this->model->DropItem(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME);
	}
	else if (key == SDLK_1) {
		this->model->DropItem(OneUpItem::ONE_UP_ITEM_NAME);
	}
	else if (key == SDLK_p) {
		this->model->TogglePauseGame();
	}
	else if (key == SDLK_w) {
		GameDisplay::ToggleDrawDebugBounds();
	}
	else if (key == SDLK_e) {
		GameDisplay::ToggleDrawDebugLightGeometry();
	}
	else if (key == SDLK_x) {
		this->model->DropItem(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME);
	}
	//else if (key == SDLK_6) {
	//	// Highlight threshold for bloom
	//	DEBUG_highlightThreshold += 0.01f;
	//}
	//else if (key == SDLK_y) {
	//	// Highlight threshold for bloom
	//	DEBUG_highlightThreshold -= 0.01f;
	//}
	//else if (key == SDLK_7) {
	//	// Scene Intensity for bloom
	//	DEBUG_sceneIntensity += 0.01f;
	//}
	//else if (key == SDLK_u) {
	//	// Scene Intensity for bloom
	//	DEBUG_sceneIntensity -= 0.01f;
	//}
	//else if (key == SDLK_8) {
	//	// Glow intensity for bloom
	//	DEBUG_glowIntensity += 0.01f;
	//}
	//else if (key == SDLK_i) {
	//	// Glow intensity for bloom
	//	DEBUG_glowIntensity -= 0.01f;
	//}
	//else if (key == SDLK_9) {
	//	// Highlight intensity for bloom
	//	DEBUG_highlightIntensity += 0.01f;
	//}
	//else if (key == SDLK_o) {
	//	// Highlight intensity for bloom
	//	DEBUG_highlightIntensity -= 0.01f;
	//}

	//else if (key == SDLK_0) {
		// Toggle multisampling
	//	FBOManager::GetInstance()->SetAllowMultisamplingFBO(!FBOManager::GetInstance()->GetAllowMultisamplingFBO());
	//}
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