/**
 * KeyboardSDLController.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "KeyboardSDLController.h"
#include "GameControl.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameDisplay.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/BallBoostModel.h"

const double KeyboardSDLController::TIME_TO_MAX_SPEED = 0.33;

KeyboardSDLController::KeyboardSDLController(GameModel* model, GameDisplay* display) :
BBBGameController(model, display), specialDirOn(false), dirHeldDownTimeCounter(0.0) {
	for (int i = 0; i < SDLK_LAST; i++) {
		this->keyPressed[i] = false;
	}
}

KeyboardSDLController::~KeyboardSDLController() {
}

bool KeyboardSDLController::ProcessState() {
	SDL_Event keyEvent;
	// Grab all the events off the queue
	while (SDL_PollEvent(&keyEvent)) {
		switch (keyEvent.type) {

			case SDL_KEYDOWN:
				this->KeyDown(keyEvent.key.keysym.sym);
				break;

			case SDL_KEYUP:
				this->KeyUp(keyEvent.key.keysym.sym);
				break;

            case SDL_MOUSEBUTTONDOWN:
                this->MouseButtonDown(keyEvent.button.button, keyEvent.button.x, keyEvent.button.y);
                break;

            case SDL_MOUSEBUTTONUP:
                this->MouseButtonUp(keyEvent.button.button);
                break;

            case SDL_MOUSEMOTION:
                this->MouseMotion(keyEvent.motion.x, keyEvent.motion.y,
                    keyEvent.motion.xrel, keyEvent.motion.yrel);
                break;

			case SDL_QUIT:
				return true;

			default:
				break;
		}
	}

	return false;
}

void KeyboardSDLController::Sync(size_t frameID, double dT) {
	UNUSED_PARAMETER(dT);

    static const float ADDED_PADDLE_MOVE_MAG = 0.2f;

	// Paddle controls (NOTE: the else is to make the feedback more exact)
    if (this->keyPressed[SDLK_LEFT] || this->keyPressed[SDLK_a]) {
		PlayerPaddle::PaddleMovement leftDir = this->model->AreControlsFlipped() ? PlayerPaddle::RightPaddleMovement : PlayerPaddle::LeftPaddleMovement;
        this->model->MovePaddle(frameID, leftDir, std::min<float>(1.0, ADDED_PADDLE_MOVE_MAG + this->dirHeldDownTimeCounter / KeyboardSDLController::TIME_TO_MAX_SPEED));
        this->dirHeldDownTimeCounter += dT;
	}
	else if (this->keyPressed[SDLK_RIGHT] || this->keyPressed[SDLK_d]) {
		PlayerPaddle::PaddleMovement rightDir = this->model->AreControlsFlipped() ? PlayerPaddle::LeftPaddleMovement : PlayerPaddle::RightPaddleMovement;
		this->model->MovePaddle(frameID, rightDir, std::min<float>(1.0, ADDED_PADDLE_MOVE_MAG + this->dirHeldDownTimeCounter / KeyboardSDLController::TIME_TO_MAX_SPEED));
        this->dirHeldDownTimeCounter += dT;
	}
	else {
		this->model->MovePaddle(frameID, PlayerPaddle::NoPaddleMovement, 0.0);
	}

	// Execute any debug functionality for when a button is held down...
	this->DebugRepeatActions();
}

void KeyboardSDLController::KeyDown(SDLKey key) {
	if (key < 0 || key > SDLK_LAST) { return; }
	this->SetKeyPress(key, true);

	this->ExecuteDisplayKeyPressedNotifications(key);
	if (key == SDLK_SPACE) {
		this->model->ShootActionReleaseUse();
	}

	// Execute any debug functionality for when a key is pressed down...
	this->DebugKeyDownActions(key);
}

void KeyboardSDLController::KeyUp(SDLKey key) {
	if (key < 0 || key > SDLK_LAST) { return; }
	this->SetKeyPress(key, false);

	this->ExecuteDisplayKeyReleasedNotifications(key);
}

void KeyboardSDLController::MouseButtonDown(unsigned int button, unsigned int x, unsigned int y) {

    // Always relay the signal to the display
    switch (button) {
        case SDL_BUTTON_LEFT:
            this->display->MousePressed(GameControl::LeftMouseButton);
            break;

        case SDL_BUTTON_RIGHT:
            this->display->MousePressed(GameControl::RightMouseButton);
            break;
        default:
            break;
    }

    if (this->model->GetCurrentStateType() != GameState::BallInPlayStateType) {
        return;
    }
    const BallBoostModel* boostModel = this->model->GetBallBoostModel();
    if (boostModel == NULL) {
        return;
    }

    unsigned int openGLYCoord = this->display->GetCamera().GetWindowHeight() - y;
    const Camera& camera = this->display->GetCamera();
    Vector2D windowCenterPos(camera.GetWindowWidth()/2, camera.GetWindowHeight()/2);
    Vector2D boostDir = Vector2D(x, openGLYCoord) - windowCenterPos;
    if (boostDir.IsZero()) {
        boostDir[0] = 1;
    }
    
    switch (button) {
        case SDL_BUTTON_LEFT:
            if (boostModel->IsInBulletTime()) {
                this->model->ShootActionReleaseUse();
            }
            else {
                this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
            }
            this->display->MousePressed(GameControl::LeftMouseButton);
            //debug_output("Left button pressed.");
            break;

        case SDL_BUTTON_RIGHT:
            if (boostModel->IsInBulletTime()) {
                this->model->ShootActionReleaseUse();
            }
            else {
                this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
            }
            this->display->MousePressed(GameControl::RightMouseButton);
            //debug_output("Right button pressed.");
            break;
        default:
            break;
    }
}

void KeyboardSDLController::MouseButtonUp(unsigned int button) {

    switch (button) {
        case SDL_BUTTON_LEFT:
            this->display->SpecialDirectionReleased();
            this->display->MouseReleased(GameControl::LeftMouseButton);
            //debug_output("Left button released.");
            break;
        case SDL_BUTTON_RIGHT:
            this->display->MouseReleased(GameControl::RightMouseButton);
            //debug_output("Right button released.");

            break;
        default:
            break;
    }
}

/**
 * Triggered when there's mouse motion, (x,y) are in screen coordinates with the 
 * upper-left corner as the origin.
 */
void KeyboardSDLController::MouseMotion(unsigned int x, unsigned int y, int relX, int relY) {
    UNUSED_PARAMETER(relX);
    UNUSED_PARAMETER(relY);

    // Convert to OpenGL screen-space coordinates system (origin in lower-left corner)...
    unsigned int openGLYCoord = this->display->GetCamera().GetWindowHeight() - y;

    if (this->model->GetCurrentStateType() == GameState::BallInPlayStateType) {
        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel == NULL) {
            return;
        }

        if (boostModel->IsInBulletTime()) {
            Vector2D previousBoostDir = boostModel->GetBallBoostDirection();

            // Calculate the angular difference from the previous mouse position to the current,
            // this, added to the previous boost direction, determines the special direction used for ball boosting
            const Camera& camera = this->display->GetCamera();

            Vector2D windowCenterPos(camera.GetWindowWidth()/2, camera.GetWindowHeight()/2);
            Vector2D boostDir = Vector2D(x, openGLYCoord) - windowCenterPos;
            if (boostDir.IsZero()) {
                boostDir[0] = 1;
            }

            this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
        }
    }
}

/**
 * Informs the display about a key pressed event in this controller.
 */
void KeyboardSDLController::ExecuteDisplayKeyPressedNotifications(SDLKey key) {

	switch (key) {
		case SDLK_DOWN:
			this->display->ButtonPressed(GameControl::DownButtonAction);
			break;
		case SDLK_UP:
			this->display->ButtonPressed(GameControl::UpButtonAction);
			break;

		case SDLK_LEFT:
        case SDLK_a:
        case SDLK_LEFTBRACKET:
		case SDLK_LEFTPAREN:
		case SDLK_KP4:
            this->display->ButtonPressed(GameControl::LeftButtonAction);
            this->dirHeldDownTimeCounter = 0.0;
			break;

		case SDLK_RIGHT:
        case SDLK_d:
		case SDLK_RIGHTBRACKET:
		case SDLK_RIGHTPAREN:
		case SDLK_KP6:
			this->display->ButtonPressed(GameControl::RightButtonAction);
            this->dirHeldDownTimeCounter = 0.0;
			break;

		case SDLK_RETURN:
			this->display->ButtonPressed(GameControl::EnterButtonAction);
			break;

		case SDLK_ESCAPE:
			this->display->ButtonPressed(GameControl::EscapeButtonAction);
			break;

		default:
			this->display->ButtonPressed(GameControl::OtherButtonAction);
			break;
	}
}

/**
 * Informs the display about a key released event in this controller.
 */
void KeyboardSDLController::ExecuteDisplayKeyReleasedNotifications(SDLKey key) {

	switch (key) {
		case SDLK_DOWN:
			this->display->ButtonReleased(GameControl::DownButtonAction);
			break;
		case SDLK_UP:
			this->display->ButtonReleased(GameControl::UpButtonAction);
			break;

		case SDLK_LEFT:
		case SDLK_LEFTBRACKET:
		case SDLK_LEFTPAREN:
		case SDLK_KP4:
			this->display->ButtonReleased(GameControl::LeftButtonAction);
            this->dirHeldDownTimeCounter = 0.0;
			break;

		case SDLK_RIGHT:
		case SDLK_RIGHTBRACKET:
		case SDLK_RIGHTPAREN:
		case SDLK_KP6:
			this->display->ButtonReleased(GameControl::RightButtonAction);
            this->dirHeldDownTimeCounter = 0.0;
			break;

		case SDLK_RETURN:
			this->display->ButtonReleased(GameControl::EnterButtonAction);
			break;

		case SDLK_ESCAPE:
			this->display->ButtonReleased(GameControl::EscapeButtonAction);
			break;

		default:
			this->display->ButtonReleased(GameControl::OtherButtonAction);
			break;
	}
}

void KeyboardSDLController::DebugRepeatActions() {
#ifdef _DEBUG
	// Camera debug functionality ....
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

void KeyboardSDLController::DebugKeyDownActions(SDLKey key) {
    UNUSED_PARAMETER(key);
    
    // Debug Item drops
#ifdef _DEBUG
    if (SDL_GetModState() & KMOD_SHIFT) {

        switch (key) {
            case SDLK_a:
                this->model->DropItem(GameItem::PaddleGrowItem);
                break;
            case SDLK_b:
                this->model->DropItem(GameItem::BallCamItem);
                break;
            case SDLK_c:
                this->model->DropItem(GameItem::PaddleCamItem);
                break;
            case SDLK_d:
                this->model->DropItem(GameItem::LaserBeamPaddleItem);
                break;
            case SDLK_e:
                GameDisplay::ToggleDrawDebugLightGeometry();
                break;
            case SDLK_f:
                this->model->DropItem(GameItem::UpsideDownItem);
                break;
            case SDLK_g:
                this->model->DropItem(GameItem::GhostBallItem);
                break;
            case SDLK_h:
                this->model->DropItem(GameItem::GravityBallItem);
                break;
            case SDLK_i:
                this->model->DropItem(GameItem::InvisiBallItem);
                break;
            case SDLK_j:
                this->model->DropItem(GameItem::CrazyBallItem);
                break;
            case SDLK_k:
                this->model->DropItem(GameItem::BallGrowItem);
                break;
            case SDLK_l:
                this->model->DropItem(GameItem::LaserBulletPaddleItem);
                break;
            case SDLK_m:
                this->model->DropItem(GameItem::BallShrinkItem);
                break;
            case SDLK_n:
                this->model->DropItem(GameItem::BallSafetyNetItem);
                break;
            case SDLK_o:
                this->model->DropItem(GameItem::ShieldPaddleItem);
                break;
            case SDLK_p:
                this->model->TogglePause(GameModel::PauseGame);
                break;
            case SDLK_q:
                this->model->DropItem(GameItem::BallSpeedUpItem);
                break;
            case SDLK_r:
                this->model->DropItem(GameItem::RocketPaddleItem);
                break;
            case SDLK_s:
                this->model->DropItem(GameItem::BallSlowDownItem);
                break;
            case SDLK_t:
                this->model->DropItem(GameItem::FireBallItem);
                break;
            case SDLK_u:
                this->model->DropItem(GameItem::UberBallItem);
                break;
            case SDLK_v:
                this->model->DropItem(GameItem::IceBallItem);
                break;
            case SDLK_w:
                GameDisplay::ToggleDrawDebugBounds();
                break;
            case SDLK_x:
                this->model->DropItem(GameItem::PoisonPaddleItem);
                break;
            case SDLK_y:
                this->model->DropItem(GameItem::StickyPaddleItem);
                break;
            case SDLK_z:
                this->model->DropItem(GameItem::PaddleShrinkItem);
                break;

            case SDLK_0:
                this->model->DropItem(GameItem::BlackoutItem);
                break;
            case SDLK_1:
                this->model->DropItem(GameItem::LifeUpItem);
                break;
            case SDLK_2:
                this->model->DropItem(GameItem::InvisiPaddleItem);
                break;
            case SDLK_3:
                this->model->DropItem(GameItem::MultiBall3Item);
                break;
            case SDLK_4:
                this->model->DropItem(GameItem::MagnetPaddleItem);
                break;
            case SDLK_5:
                this->model->DropItem(GameItem::MultiBall5Item);
                break;
            case SDLK_6:
                this->model->DropItem(GameItem::MineLauncherPaddleItem);
                break;
            case SDLK_7:
                // ...
                break;
            case SDLK_8:
                // ...
                break;
            case SDLK_9:
                this->model->DropItem(GameItem::OmniLaserBallItem);
                break;
            

            case SDLK_BACKSLASH:
                this->model->DropItem(GameItem::RandomItem);
                break;
            case SDLK_SLASH:
                GameDisplay::ToggleDetachCamera();

                break;
            case SDLK_COMMA:
                this->model->TogglePause(GameModel::PauseBall);
                break;
            
            
            default:
                break;
        }

#ifdef _DEBUG
        // Shift + Ctrl
        if (SDL_GetModState() & KMOD_CTRL) {
            switch (key) {
                case SDLK_BACKSLASH:
                    this->display->ButtonPressed(GameControl::SpecialCheatButtonAction);
                    break;

                default:
                    break;
            }
        }
#endif

    }
#endif
}