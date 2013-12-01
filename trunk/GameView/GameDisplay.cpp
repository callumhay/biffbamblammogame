/**
 * GameDisplay.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
#include "CreditsDisplayState.h"
#include "LoadingScreen.h"
#include "MouseRenderer.h"
#include "MenuBackgroundRenderer.h"

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

GameDisplay::GameDisplay(GameModel* model, GameSound* sound, int initWidth, int initHeight): 
gameListener(NULL), currState(NULL), mouseRenderer(NULL), menuBGRenderer(NULL), model(model), 
assets(NULL), sound(sound), gameExited(false), gameReinitialized(false), gameCamera() {

	assert(model != NULL);
    assert(sound != NULL);

    // MAKE SURE WE SET THE CAMERA DIMENSIONS!!!!
    Camera::SetWindowDimensions(initWidth, initHeight);

    this->assets = new GameAssets(initWidth, initHeight, this->sound);
    this->mouseRenderer = new MouseRenderer();
    this->menuBGRenderer = new MenuBackgroundRenderer();

	this->SetupActionListeners();
	this->SetCurrentState(new MainMenuDisplayState(this));
    //this->SetCurrentState(new BossLevelCompleteSummaryDisplayState(this)); // Testing world complete screen
    //this->SetCurrentState(new GameCompleteDisplayState(this)); // Testing game complete screen
	SetInitialRenderOptions();
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
    this->model->BallBoostDirectionPressed(-x, -y);
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