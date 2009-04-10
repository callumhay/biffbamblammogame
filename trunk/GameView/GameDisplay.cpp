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

#include "../BlammoEngine/BlammoEngine.h"

float tempRot = 0;

const int GameDisplay::MAX_FRAMERATE						= 500;
const unsigned long GameDisplay::FRAME_SLEEP_MS	= 1000 / GameDisplay::MAX_FRAMERATE;

GameDisplay::GameDisplay(GameModel* model, int initWidth, int initHeight): gameListener(NULL), currState(NULL),
model(model), assets(new GameAssets()), width(initWidth), height(initHeight), gameExited(false) {
	assert(model != NULL);

	this->SetupActionListeners();
	this->SetCurrentState(new MainMenuDisplayState(this));

#ifdef _DEBUG
	this->drawDebug = false;
#endif
}

GameDisplay::~GameDisplay() {
	// Delete game assets
	delete this->assets;
	this->assets = NULL;
	
	// Delete any current state
	assert(this->currState != NULL);
	if (this->currState != NULL) {
		delete this->currState;
		this->currState = NULL;
	}

	// Remove any action listeners for the model
	this->RemoveActionListeners();
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
	this->width = w;
	this->height = h;
	this->gameCamera.SetPerspective(w, h);
	this->currState->DisplaySizeChanged(w, h);
}

void GameDisplay::Render(double dT) {
	SetInitialRenderOptions();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Render the current state
	this->currState->RenderFrame(dT);
	debug_opengl_state();

	// Update the game model
	this->model->Tick(dT);
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

// DEBUG FUNCTIONS ******************************************************
#ifdef _DEBUG
/*
 * Draws the x, y and z axes as red, yellow and blue lines, respectively.
 * The thicker half of the line is the positive direction.
 * Precondition: true.
 */
void GameDisplay::DrawDebugAxes() {
	if (!drawDebug) {return;}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glLoadIdentity();

	glLineWidth(1.0f);
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(-100.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
	glEnd();

	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(100.0f,0.0f,0.0f);
	glEnd();

	glColor3f(0,0,1);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,-100.0f);
		glVertex3f(0.0f,0.0f,0.0f);
	glEnd();

	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,100.0f);
	glEnd();

	glColor3f(1,1,0);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,-100.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
	glEnd();

	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,100.0f,0.0f);
	glEnd();

	// Reset line width and colour
	glLineWidth(1.0f);
	glColor3f(0,0,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

/*
 * Draw a set of grids, each square of which is a unit squared. The given parameters
 * say which plane(s) to draw the grid on and the number of ticks on each grid.
 * Precondition: numGridTicks > 0.
 */
void GameDisplay::DrawDebugUnitGrid(bool xy, bool xz, bool zy, int numGridTicks) {
	assert(numGridTicks > 0);
	if (!drawDebug) {return;}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0, 1, 0);

	glPushMatrix();
	glLoadIdentity();

	if (xy) {
		for (int i = -numGridTicks; i <= numGridTicks; i++) {
			glBegin(GL_LINES);
			glVertex3f(i, numGridTicks, 0);
			glVertex3f(i, -numGridTicks, 0);
			glVertex3f(numGridTicks, i, 0);
			glVertex3f(-numGridTicks, i, 0);
			glEnd();
		}
	}

	if (xz) {
		for (int i = -numGridTicks; i <= numGridTicks; i++) {
			glBegin(GL_LINES);
			glVertex3f(i, 0, numGridTicks);
			glVertex3f(i, 0, -numGridTicks);
			glVertex3f(numGridTicks, 0, i);
			glVertex3f(-numGridTicks, 0, i);
			glEnd();
		}
	}

	if (zy) {
		for (int i = -numGridTicks; i <= numGridTicks; i++) {
			glBegin(GL_LINES);
			glVertex3f(0, i, numGridTicks);
			glVertex3f(0, i, -numGridTicks);
			glVertex3f(0, numGridTicks, i);
			glVertex3f(0, -numGridTicks, i);
			glEnd();
		}
	}

	// Reset the colour
	glColor3f(0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}
#endif