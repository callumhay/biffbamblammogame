/**
 * GameDisplay.cpp
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

// GameDisplay Includes
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameEventsListener.h"
#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "MainMenuDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "BossLevelCompleteSummaryDisplayState.h"
#include "LevelCompleteSummaryDisplayState.h"
#include "CreditsDisplayState.h"
#include "LoadingScreen.h"
#include "MouseRenderer.h"
#include "MenuBackgroundRenderer.h"
#include "GameViewEventManager.h"

// Model includes
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameEventManager.h"

#ifdef _DEBUG
bool GameDisplay::drawDebugBounds           = false;
bool GameDisplay::drawDebugLightGeometry    = false;
bool GameDisplay::detachedCamera            = false;
#endif

const int GameDisplay::MAX_FRAMERATE = 500;
const unsigned long GameDisplay::FRAME_SLEEP_MS	= 1000 / GameDisplay::MAX_FRAMERATE;

bool GameDisplay::arcadeMode = false;

GameDisplay::GameDisplay(GameModel* model, GameSound* sound, int initWidth, int initHeight, bool arcadeMode): 
gameListener(NULL), currState(NULL), mouseRenderer(NULL), menuBGRenderer(NULL), model(model), 
assets(NULL), sound(sound), gameExited(false), gameReinitialized(false), gameCamera() {
    
    GameDisplay::arcadeMode = arcadeMode;

	assert(model != NULL);
    assert(sound != NULL);

    this->assets = new GameAssets(initWidth, initHeight, this->sound);
    this->mouseRenderer = new MouseRenderer();
    this->menuBGRenderer = new MenuBackgroundRenderer();

	this->SetupActionListeners();
	this->SetCurrentState(new MainMenuDisplayState(this));
    //this->SetCurrentState(new LevelCompleteSummaryDisplayState(this)); // Testing level complete screen
    //this->SetCurrentState(new BossLevelCompleteSummaryDisplayState(this)); // Testing world complete screen
    //this->SetCurrentState(new GameCompleteDisplayState(this)); // Testing game complete screen
	this->SetInitialRenderOptions();
}

GameDisplay::~GameDisplay() {

	// Delete any current state
	assert(this->currState != NULL);
	if (this->currState != NULL) {

        // Annoying: If the game is exited abruptly from the in-game menu then we need
        // to make sure we clean up the game state that it temporarily stores as well
        if (this->currState->GetType() == DisplayState::InGameMenu) {
            static_cast<InGameMenuState*>(this->currState)->CleanUpReturnToDisplayState();
        }

		delete this->currState;
		this->currState = NULL;
	}

	// Remove any action listeners for the model
	this->RemoveActionListeners();

    delete this->mouseRenderer;
    this->mouseRenderer = NULL;

    delete this->menuBGRenderer;
    this->menuBGRenderer = NULL;

	// Delete game assets LAST!! (If you don't do this last
	// then all the other things being destroyed in this destructor
	// will try to access the assets that no longer exist).
	delete this->assets;
	this->assets = NULL;
}

// RENDER FUNCTIONS ****************************************************

void GameDisplay::SetInitialRenderOptions() {
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void GameDisplay::SetCurrentState(DisplayState* state) {
    assert(state != NULL);
    DisplayState::DisplayStateType prevType = DisplayState::NoState;
    if (this->currState != NULL) {
        prevType = this->currState->GetType();
        delete this->currState;
    }
    this->currState = state;

    // EVENT: The game display state just changed
    GameViewEventManager::Instance()->ActionDisplayStateChanged(prevType, this->currState->GetType());
}

// Sets the current state to be the next state lined up on the
// state queue - these get pushed on as the game model sends events to the view
// and as the game view states progress
bool GameDisplay::SetCurrentStateAsNextQueuedState() {
    if (this->stateQueue.empty()) {
        return false;
    }

    std::pair<DisplayState::DisplayStateType, DisplayStateInfo> nextStateTypePair = this->stateQueue.front();
	this->stateQueue.pop_front();
	this->SetCurrentState(DisplayState::BuildDisplayStateFromType(nextStateTypePair.first, nextStateTypePair.second, this));
    return true;
}

void GameDisplay::ChangeDisplaySize(int w, int h) {
	this->gameCamera.SetWindowDimensions(w, h);
	this->gameCamera.SetPerspective();
	this->currState->DisplaySizeChanged(w, h);
}

float GameDisplay::GetTextScalingFactor() {
	// We choose a base resolution to scale from...
	static const float BASE_X_RESOLUTION = 1152;
	static const float BASE_Y_RESOLUTION = 864;
	
    float currXRes = Camera::GetWindowWidth();
	float currYRes = Camera::GetWindowHeight();

	return std::min<float>(currXRes / BASE_X_RESOLUTION, currYRes / BASE_Y_RESOLUTION);
}

void GameDisplay::SpecialDirectionPressed(float x, float y) {
    this->model->BallBoostDirectionPressed(-x, -y, GameDisplay::arcadeMode);
}

void GameDisplay::SpecialDirectionReleased() {
    this->model->BallBoostDirectionReleased();
}

// LISTENER FUNCTIONS ***************************************************
void GameDisplay::SetupActionListeners() {
	this->gameListener = new GameEventsListener(this);
	GameEventManager::Instance()->RegisterGameEventListener(this->gameListener);
}
void GameDisplay::RemoveActionListeners() {
	GameEventManager::Instance()->UnregisterGameEventListener(this->gameListener);
	delete this->gameListener;
}