/**
 * KeyboardSDLController.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "KeyboardSDLController.h"
#include "GameControl.h"

#include "../BlammoEngine/Camera.h"
#include "../GameView/GameDisplay.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/BallBoostModel.h"

const double KeyboardSDLController::TIME_TO_MAX_SPEED = 0.2;

KeyboardSDLController::KeyboardSDLController(GameModel* model, GameDisplay* display) :
BBBGameController(model, display), dirHeldDownTimeCounter(0.0), windowHasFocus(true) {
	for (int i = 0; i < SDLK_LAST; i++) {
		this->keyPressed[i] = false;
	}
}

KeyboardSDLController::~KeyboardSDLController() {
}

bool KeyboardSDLController::ProcessState(double dT) {
    UNUSED_PARAMETER(dT);

	SDL_Event keyEvent;
	// Grab all the events off the queue
	while (SDL_PollEvent(&keyEvent)) {
		switch (keyEvent.type) {

            case SDL_ACTIVEEVENT:
                this->WindowActiveEvent(keyEvent.active);
                break;

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

    float magnitudePercent = std::min<float>(1.0, ADDED_PADDLE_MOVE_MAG + this->dirHeldDownTimeCounter / KeyboardSDLController::TIME_TO_MAX_SPEED);

	// Movement controls (NOTE: the else is to make the feedback more exact)
    if (this->keyPressed[SDLK_LEFT] || this->keyPressed[SDLK_a]) {
		
        int leftDir = this->model->AreControlsFlippedForPaddle() ? 1 : -1;
        this->model->MovePaddle(frameID, leftDir, magnitudePercent);

        leftDir = this->model->AreControlsFlippedForOther() ? 1 : -1;
        this->model->MoveOther(frameID, leftDir, magnitudePercent);

        this->dirHeldDownTimeCounter += dT;
	}
	else if (this->keyPressed[SDLK_RIGHT] || this->keyPressed[SDLK_d]) {
		
        int rightDir = this->model->AreControlsFlippedForPaddle() ? -1 : 1;
        this->model->MovePaddle(frameID, rightDir, magnitudePercent);

        rightDir = this->model->AreControlsFlippedForOther() ? -1 : 1;
        this->model->MoveOther(frameID, rightDir, magnitudePercent);

        this->dirHeldDownTimeCounter += dT;
	}
	else {
		this->model->MovePaddle(frameID, 0, 0.0);
        this->model->MoveOther(frameID, 0, 0.0);
	}

    // Other special actions for hold-down keys...
    if (this->keyPressed[SDLK_SPACE] || this->keyPressed[SDLK_UP] || this->keyPressed[SDLK_w]) {
        this->model->ShootActionContinuousUse(dT, 1.0f);
    }

	// Execute any debug functionality for when a button is held down...
	this->DebugRepeatActions();
}

void KeyboardSDLController::KeyDown(SDLKey key) {
	if (key < 0 || key > SDLK_LAST) { return; }
	this->SetKeyPress(key, true);

	this->ExecuteDisplayKeyPressedNotifications(key);
	if (key == SDLK_SPACE || key == SDLK_UP || key == SDLK_w) {
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

    // If the game is not in-play then we do nothing with the boost model...
    if (DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState())) {

        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel == NULL) {
            return;
        }

        unsigned int openGLYCoord = Camera::GetWindowHeight() - y;
        Vector2D windowCenterPos(Camera::GetWindowWidth()/2, Camera::GetWindowHeight()/2);
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
                break;

            case SDL_BUTTON_RIGHT:
                if (boostModel->IsInBulletTime()) {
                    this->model->ShootActionReleaseUse();
                }
                else {
                    this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
                }
                break;

            default:
                break;
        }
    }
    
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
}

void KeyboardSDLController::MouseButtonUp(unsigned int button) {

    if (DisplayState::IsGameInPlayDisplayState(this->display->GetCurrentDisplayState())) {
        switch (button) {
            case SDL_BUTTON_LEFT:
                // Release the boost...
                this->display->SpecialDirectionReleased();
                break;

            default:
                break;
        }
    }

    switch (button) {
        case SDL_BUTTON_LEFT:
            this->display->MouseReleased(GameControl::LeftMouseButton);
            break;
        case SDL_BUTTON_RIGHT:
            this->display->MouseReleased(GameControl::RightMouseButton);
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

    if (this->windowHasFocus && this->model->GetCurrentStateType() == GameState::BallInPlayStateType) {

        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel == NULL) {
            return;
        }

        if (boostModel->IsInBulletTime()) {
            Vector2D windowCenterPos(Camera::GetWindowWidth()/2, Camera::GetWindowHeight()/2);

            // Calculate the angular difference from the previous mouse position to the current,
            // this, added to the previous boost direction, determines the special direction used for ball boosting
            Vector2D boostDir = Vector2D(x, openGLYCoord) - windowCenterPos;
            if (boostDir.IsZero()) {
                boostDir[0] = 1;
            }

            this->display->SpecialDirectionPressed(boostDir[0], boostDir[1]);
        }
    }

    // Tell the display that there was mouse motion...
    this->display->MouseMoved(x, openGLYCoord);
}

void KeyboardSDLController::WindowActiveEvent(const SDL_ActiveEvent& sdlActiveEvent) {
    enum ApplicationWindowState {
        Minimized,   // application window has been iconified
        Restored,    // application window has been restored
        GainedFocus, // application window got input focus
        LostFocus    // application window lost input focus
    };

    static ApplicationWindowState appWindowState = Restored;

    if (sdlActiveEvent.state == (SDL_APPINPUTFOCUS | SDL_APPACTIVE)) {
        if (sdlActiveEvent.gain == 0) {
            this->windowHasFocus = false;
            appWindowState = Minimized;
            debug_output("Window Minimized");
        }
        else {
            this->windowHasFocus = true;

            if (appWindowState == LostFocus) {    
                appWindowState = GainedFocus;
                debug_output("Window Gained Focus");
            }
            else {
                appWindowState = Restored;
                debug_output("Window Restored");
            }
        }
    }
    else if ((sdlActiveEvent.state & SDL_APPINPUTFOCUS) == SDL_APPINPUTFOCUS) {
        if (sdlActiveEvent.gain == 0) {
            this->windowHasFocus = false;
            appWindowState = LostFocus;
            debug_output("Window Lost Focus");
        }
        else {
            this->windowHasFocus = true;
            debug_output("Window Gained Focus");
            appWindowState = GainedFocus;
        }
    }

    if (!this->windowHasFocus) {
        // If we just lost focus then we need to stop any ball boosting...
        const BallBoostModel* boostModel = this->model->GetBallBoostModel();
        if (boostModel != NULL) {
            this->display->GetModel()->ReleaseBulletTime();
        }
    }

    this->display->WindowFocus(windowHasFocus);
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
    static const float MOVE_MULTIPLIER = 0.1f;
	// Camera debug functionality ....
	if (this->keyPressed[SDLK_KP8]) {
		this->display->GetCamera().Move(MOVE_MULTIPLIER*Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed[SDLK_KP5]) {
		this->display->GetCamera().Move(-MOVE_MULTIPLIER*Camera::DEFAULT_FORWARD_VEC);
	}
	else if (this->keyPressed[SDLK_KP4]) {
		this->display->GetCamera().Move(MOVE_MULTIPLIER*Camera::DEFAULT_LEFT_VEC);
	}
	else if (this->keyPressed[SDLK_KP6]) {
		this->display->GetCamera().Move(-MOVE_MULTIPLIER*Camera::DEFAULT_LEFT_VEC);
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

        // Shift + Ctrl
        if (SDL_GetModState() & KMOD_CTRL) {
            switch (key) {
                case SDLK_BACKSLASH:
                    this->display->ButtonPressed(GameControl::SpecialCheatButtonAction);
                    break;
                case SDLK_3:
                    this->model->DropThreeItems(GameItem::LaserBeamPaddleItem, GameItem::MagnetPaddleItem, GameItem::UpsideDownItem);
                default:
                    break;
            }
        }
        else {
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
                    this->model->DropItem(GameItem::RemoteCtrlRocketItem);
                    break;
                case SDLK_8:
                    // ...
                    break;
                case SDLK_9:
                    this->model->DropItem(GameItem::OmniLaserBallItem);
                    break;
                
                case SDLK_MINUS:
                case SDLK_UNDERSCORE:
                    this->model->DropItem(GameItem::FlameBlasterPaddleItem);
                    break;
                case SDLK_EQUALS:
                case SDLK_PLUS:
                    this->model->DropItem(GameItem::IceBlasterPaddleItem);
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
                case SDLK_PERIOD:
                    this->model->TogglePause(GameModel::PauseAI);
                    break;

                case SDLK_QUOTEDBL:
                case SDLK_QUOTE:
                    this->display->ReloadSound();
                    break;

                default:
                    break;
            }
        }
    }
#endif
}