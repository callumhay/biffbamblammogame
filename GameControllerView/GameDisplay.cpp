#include "GameDisplay.h"
#include "ObjReader.h"
#include "Mesh.h"
#include "GameAssets.h"
#include "Camera.h"
#include "GameEventsListener.h"

#include "CgShaderManager.h"

// State includes
#include "InGameDisplayState.h"
#include "MainMenuDisplayState.h"

// Model includes
#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameEventManager.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

float tempRot = 0;
const double GameDisplay::FRAME_DT_SEC = 1.0/GameDisplay::FRAMERATE;
const double GameDisplay::FRAME_DT_MILLISEC = 1000.0/GameDisplay::FRAMERATE;

GameDisplay::GameDisplay(GameModel* model, int initWidth, int initHeight): gameListener(NULL), currState(NULL),
model(model), assets(new GameAssets()), width(initWidth), height(initHeight) {
	
	assert(model != NULL);

	this->SetupActionListeners();
	this->SetCurrentState(new MainMenuDisplayState(this));
}

GameDisplay::~GameDisplay() {
	delete this->assets;
	this->RemoveActionListeners();
}

// RENDER FUNCTIONS ****************************************************

void GameDisplay::SetInitialRenderOptions() {
	glEnable(GL_NORMALIZE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);

	glDisable(GL_LIGHTING);
}

// Set the opengl state for drawing celshading outlines
void GameDisplay::SetOutlineRenderAttribs(float outlineWidth) {
	glDisable(GL_LIGHTING);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode (GL_BACK, GL_LINE);
	glColor3f(0,0,0);
	glLineWidth(outlineWidth);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void GameDisplay::ChangeDisplaySize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) {
		h = 1;
	}

	glViewport(0, 0, w, h);
	this->width = w;
	this->height = h;
}

void GameDisplay::Render() {
	SetInitialRenderOptions();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

	// Render the current state
	this->currState->RenderFrame(GameDisplay::FRAME_DT_SEC);
	//this->DrawDebugAxes();

	glutSwapBuffers();

	// Update the game model
	this->model->Tick(GameDisplay::FRAME_DT_SEC);
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

/*
 * Draws the x, y and z axes as red, yellow and blue lines, respectively.
 * The thicker half of the line is the positive direction.
 * Precondition: true.
 */
void GameDisplay::DrawDebugAxes() {
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