/**
 * KeyboardSDLController.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
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

KeyboardSDLController::KeyboardSDLController(GameModel* model, GameDisplay* display) :
BBBGameController(model, display), specialDirOn(false) {
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

	// Paddle controls (NOTE: the else is to make the feedback more exacting)
    if (this->keyPressed[SDLK_LEFT] || this->keyPressed[SDLK_a]) {
		PlayerPaddle::PaddleMovement leftDir = this->model->AreControlsFlipped() ? PlayerPaddle::RightPaddleMovement : PlayerPaddle::LeftPaddleMovement;
		this->model->MovePaddle(frameID, leftDir);
	}
	else if (this->keyPressed[SDLK_RIGHT] || this->keyPressed[SDLK_d]) {
		PlayerPaddle::PaddleMovement rightDir = this->model->AreControlsFlipped() ? PlayerPaddle::LeftPaddleMovement : PlayerPaddle::RightPaddleMovement;
		this->model->MovePaddle(frameID, rightDir);
	}
	else {
		this->model->MovePaddle(frameID, PlayerPaddle::NoPaddleMovement);
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
    Vector2D boostDir = Vector2D(x,openGLYCoord) - windowCenterPos;
    if (boostDir.IsZero()) {
        boostDir[0] = 1;
    }
    
    switch (button) {
        case SDL_BUTTON_LEFT:
            this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
            //debug_output("Left button pressed.");
            break;
        case SDL_BUTTON_RIGHT:
            if (boostModel->IsInBulletTime()) {
                this->model->ShootActionReleaseUse();
            }
            else {
                this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
            }
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
            //debug_output("Left button released.");
            break;
        case SDL_BUTTON_RIGHT:
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
    // Convert to OpenGL screen-space coordinates system (origin in lower-left corner)...
    unsigned int openGLYCoord = this->display->GetCamera().GetWindowHeight() - y;
    //debug_output("Mouse motion (absolute): " << x << ", " << openGLYCoord);
    int openGLRelYCoord = -relY;
    //debug_output("Mouse motion (relative): " << relX << ", " << openGLRelYCoord);

    if (this->model->GetCurrentStateType() == GameState::BallInPlayStateType) {
        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel == NULL) {
            return;
        }

        if (boostModel->IsInBulletTime()) {
            Vector2D previousBoostDir = -boostModel->GetBallBoostDirection();

            // Calculate the angular difference from the previous mouse position to the current,
            // this, added to the previous boost direction, determines the special direction used for ball boosting
            const Camera& camera = this->display->GetCamera();
            Vector2D windowCenterPos(camera.GetWindowWidth()/2, camera.GetWindowHeight()/2);

            Vector2D previousMousePos(x - relX, openGLYCoord - openGLRelYCoord);
            previousMousePos = previousMousePos - windowCenterPos;
            previousMousePos.Normalize();

            Vector2D currMousePos(x, openGLYCoord);
            currMousePos = currMousePos - windowCenterPos;
            currMousePos.Normalize();
            
            float angleInRadians = acos(Vector2D::Dot(previousMousePos, currMousePos));
            float sign = NumberFuncs::SignOf(Vector3D::cross(Vector3D(previousMousePos), Vector3D(currMousePos))[2]);
            previousBoostDir.Rotate(sign * Trig::radiansToDegrees(angleInRadians));
            previousBoostDir = 1000 * previousBoostDir;

            if (previousBoostDir.IsZero()) {
                return;
            }

            this->display->SpecialDirectionPressed(previousBoostDir[0], previousBoostDir[1]);
        }
    }
}

/**
 * Informs the display about a key pressed event in this controller.
 */
void KeyboardSDLController::ExecuteDisplayKeyPressedNotifications(SDLKey key) {

    /*
    Vector2D specialDir(0,0);
    this->GetSpecialDirectionVector(specialDir);
    if (!specialDir.IsZero()) {
        this->display->SpecialDirectionPressed(specialDir[0], specialDir[1]);
        this->specialDirOn = true;
        if (this->keyPressed[SDLK_SPACE]) {
            this->model->ShootActionReleaseUse();
        }
    }
    */

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
			break;

		case SDLK_RIGHT:
        case SDLK_d:
		case SDLK_RIGHTBRACKET:
		case SDLK_RIGHTPAREN:
		case SDLK_KP6:
			this->display->ButtonPressed(GameControl::RightButtonAction);
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

    /*
    Vector2D specialDir(0,0);
    this->GetSpecialDirectionVector(specialDir);

    if (!specialDir.IsZero()) {
        this->display->SpecialDirectionPressed(specialDir[0], specialDir[1]);
        this->specialDirOn = true;
        if (this->keyPressed[SDLK_SPACE]) {
            this->model->ShootActionReleaseUse();
        }
    }
    else if (this->specialDirOn) {
        this->display->SpecialDirectionReleased();
        this->specialDirOn = false;
    }
    */

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
			break;

		case SDLK_RIGHT:
		case SDLK_RIGHTBRACKET:
		case SDLK_RIGHTPAREN:
		case SDLK_KP6:
			this->display->ButtonReleased(GameControl::RightButtonAction);
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

    
    // Debug Item drops
#ifdef _DEBUG
    if (SDL_GetModState() & KMOD_SHIFT) {
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
	    else if (key == SDLK_t) {
		    this->model->DropItem(GameItem::FireBallItem);
	    }
	    else if (key == SDLK_u) {
		    this->model->DropItem(GameItem::UberBallItem);
	    }
	    else if (key == SDLK_v) {
		    this->model->DropItem(GameItem::IceBallItem);
	    }
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
        else if (key == SDLK_9) {
            this->model->DropItem(GameItem::OmniLaserBallItem);
        }
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
    }
#endif
}