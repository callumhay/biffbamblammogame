/**
 * GameControllerManager.cpp
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

#include "GameControllerManager.h"
#include "BBBGameController.h"
#include "KeyboardSDLController.h"
#include "ArcadeController.h"

#ifdef USE_XBOX360_CONTROLLER
#include "XBox360Controller.h"
#endif

#ifdef USE_KINECT_CONTROLLER
#include "KinectController.h"
#endif

GameControllerManager* GameControllerManager::instance = NULL;

const size_t GameControllerManager::KEYBOARD_SDL_INDEX     = 0;
const size_t GameControllerManager::ARCADE_INDEX           = KEYBOARD_SDL_INDEX;
const size_t GameControllerManager::XBOX_360_INDEX         = 1;
const size_t GameControllerManager::KINECT_INDEX           = 2;
const size_t GameControllerManager::NUM_CONTROLLER_INDICES = 3;

GameControllerManager::GameControllerManager() : model(NULL), display(NULL) {
	this->gameControllers.resize(NUM_CONTROLLER_INDICES, NULL);
}

GameControllerManager::~GameControllerManager() {
    this->ClearControllerVibration();

	// Clean up all the controllers...
	for (std::list<BBBGameController*>::iterator iter = this->loadedGameControllers.begin(); iter != this->loadedGameControllers.end(); ++iter) {
		BBBGameController* currController = *iter;
		delete currController;
		currController = NULL;
	}
	this->gameControllers.clear();
	this->loadedGameControllers.clear();
}

void GameControllerManager::InitAllControllers(GameModel* model, GameDisplay* display, bool arcadeMode) {
	assert(model != NULL);
	assert(display != NULL);
	this->model   = model;
	this->display = display;

    // NOTE: Both the keyboard AND arcade controllers CANNOT exist at the same time!!
    if (arcadeMode) {
        this->GetArcadeController();
    }
    else {
	    this->GetSDLKeyboardGameController();
    }

	this->GetXBox360Controller(0);
    this->GetKinectController();
}

void GameControllerManager::SetArcadeSerialPort(const std::string& serialPort) {
    if (this->gameControllers[ARCADE_INDEX] != NULL) {
        static_cast<ArcadeController*>(this->gameControllers[ARCADE_INDEX])->SetSerialPort(serialPort);
    }
}

/**
 * Builds a keyboard controller when using the SDL library.
 */
BBBGameController* GameControllerManager::GetSDLKeyboardGameController() {
	assert(this->model != NULL);
	assert(this->display != NULL);

	if (this->gameControllers[KEYBOARD_SDL_INDEX] == NULL) {
		BBBGameController* sdlKeyboard = new KeyboardSDLController(this->model, this->display);
		this->gameControllers[KEYBOARD_SDL_INDEX] = sdlKeyboard;
		this->loadedGameControllers.push_back(sdlKeyboard);
	}
	return this->gameControllers[KEYBOARD_SDL_INDEX];
}

BBBGameController* GameControllerManager::GetArcadeController() {
    assert(this->model != NULL);
    assert(this->display != NULL);

    if (this->gameControllers[ARCADE_INDEX] == NULL) {
        BBBGameController* arcadeController = new ArcadeController(this->model, this->display);
        this->gameControllers[ARCADE_INDEX] = arcadeController;
        this->loadedGameControllers.push_back(arcadeController);
    }
    return this->gameControllers[ARCADE_INDEX];
}

/**
 * Builds a XBox 360 Game controller when in windows using the XInput library.
 */
BBBGameController* GameControllerManager::GetXBox360Controller(int controllerNum) {
	assert(this->model != NULL);
	assert(this->display != NULL);

#if defined(USE_XBOX360_CONTROLLER)
	if (this->gameControllers[XBOX_360_INDEX] == NULL) {
		// Check for a connected XBox 360 controller of the given controller number
		if (XBox360Controller::IsConnected(controllerNum)) {
			BBBGameController* xBox360Controller = new XBox360Controller(this->model, this->display, controllerNum);
		    this->gameControllers[XBOX_360_INDEX] = xBox360Controller;
		    this->loadedGameControllers.push_back(xBox360Controller);
		}
	}

	return this->gameControllers[XBOX_360_INDEX];
#else
    return NULL;
#endif
}

BBBGameController* GameControllerManager::GetKinectController() {
    assert(this->model != NULL);
    assert(this->display != NULL);

#if defined(USE_KINECT_CONTROLLER)
    if (this->gameControllers[KINECT_INDEX] == NULL) {
        // Check for a connected Kinect controller
        if (KinectController::CheckForConnection()) {
            BBBGameController* kinectController = new KinectController(this->model, this->display);
            this->gameControllers[KINECT_INDEX] = kinectController;
            this->loadedGameControllers.push_back(kinectController);
        }
    }

    return this->gameControllers[KINECT_INDEX];
#else
    return NULL;
#endif
}

/**
 * Tries to load any newly introduced plug-and-play controllers.
 */
void GameControllerManager::TryToLoadPlugAndPlayControllers() {
	assert(this->model != NULL);
	assert(this->display != NULL);

	if (this->ControllersCanStillPlugAndPlay()) {
		// Load any plug and play controllers if possible...
		GameControllerManager::GetXBox360Controller(0);
        GameControllerManager::GetKinectController();
	}
}

void GameControllerManager::SetControllerSensitivity(int sensitivity) {
#if defined(USE_XBOX360_CONTROLLER)
    XBox360Controller::SetSensitivity(sensitivity);
#endif
}