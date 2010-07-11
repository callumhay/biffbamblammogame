#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameEventsListener.h"

// State includes
#include "InGameDisplayState.h"
#include "MainMenuDisplayState.h"

// Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameEventManager.h"

#ifdef _DEBUG
bool GameDisplay::drawDebugBounds					= false;
bool GameDisplay::drawDebugLightGeometry	= false;
bool GameDisplay::detachedCamera					= false;
#endif

const int GameDisplay::MAX_FRAMERATE						= 500;
const unsigned long GameDisplay::FRAME_SLEEP_MS	= 1000 / GameDisplay::MAX_FRAMERATE;

GameDisplay::GameDisplay(GameModel* model, int initWidth, int initHeight): 
gameListener(NULL), currState(NULL), model(model), assets(new GameAssets(initWidth, initHeight)), 
gameExited(false), gameReinitialized(false), 
gameCamera(initWidth, initHeight) {
	assert(model != NULL);

	this->SetupActionListeners();
	this->SetCurrentState(new MainMenuDisplayState(this));
	SetInitialRenderOptions();
}

GameDisplay::~GameDisplay() {

	// Delete any current state
	assert(this->currState != NULL);
	if (this->currState != NULL) {
		delete this->currState;
		this->currState = NULL;
	}

	// Remove any action listeners for the model
	this->RemoveActionListeners();

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
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_LIGHTING);
}

void GameDisplay::ChangeDisplaySize(int w, int h) {
	this->gameCamera.SetWindowDimensions(w, h);
	this->gameCamera.SetPerspective();
	this->currState->DisplaySizeChanged(w, h);
}

void GameDisplay::Render(double dT) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the current state
	this->currState->RenderFrame(dT);
	debug_opengl_state();

	// Update the game model
	this->model->Tick(dT);
	this->model->UpdateState();
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