#include "GameDisplay.h"
#include "ObjReader.h"
#include "Mesh.h"
#include "GameAssets.h"
#include "Camera.h"
#include "GameEventsListener.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameWorld.h"
#include "../GameModel/GameEventManager.h"

#include "../Utils/Includes.h"
#include "../Utils/Vector.h"

float tempRot = 0;
const double GameDisplay::FRAME_DT_SEC = 1.0/GameDisplay::FRAMERATE;
const double GameDisplay::FRAME_DT_MILLISEC = 1000.0/GameDisplay::FRAMERATE;

GameDisplay::GameDisplay(GameModel* model, int initWidth, int initHeight): gameListener(NULL),
model(model), assets(new GameAssets()), camera(new Camera(45.0f, 0.01f, 100.0f, initWidth, initHeight)) {
	assert(model != NULL);

	this->assets->LoadAssets(this->model->GetCurrentWorldStyle());
	this->SetupActionListeners();
	this->SetupRenderOptions();
}

GameDisplay::~GameDisplay() {
	delete this->assets;
	delete this->camera;
	this->RemoveActionListeners();
}

// RENDER FUNCTIONS ****************************************************

void GameDisplay::SetupRenderOptions() {
	glEnable(GL_NORMALIZE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Lighting
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHT0);
}

void GameDisplay::ChangeDisplaySize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) {
		h = 1;
	}
	this->camera->SetViewportAndProjectionTransform(w, h);
}

void GameDisplay::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	
	// Start drawing...
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -43.0f); // Eye transform

	// Draw the scene...
	glPushMatrix();

	Vector2D levelDim = this->model->GetLevelUnitDimensions();
	glTranslatef(-levelDim[0]/2.0f, -levelDim[1]/2.0f, 0.0f);

	this->DrawLevelPieces();
	this->DrawPlayerPaddle();
	this->assets->DrawGameBall(this->model->GetGameBall());

	glPopMatrix();
	tempRot += 1.0f;
	
	//GameDisplay::DrawDebugUnitGrid(true, true, true, 15);
	//GameDisplay::DrawDebugAxes();

	glutSwapBuffers();
	this->model->Tick(GameDisplay::FRAME_DT_SEC);
}

/*
 * Helper function for drawing all the game pieces / blocks that make up the currently
 * loaded level. This does not include drawing the player, ball, effects or backgrounds.
 */
void GameDisplay::DrawLevelPieces() {
	std::vector<std::vector<LevelPiece*>> &pieces = this->model->GetCurrentLevelPieces();

	// Go through each piece and draw
	for (size_t h = 0; h < pieces.size(); h++) {
		for (size_t w = 0; w < pieces[h].size(); w++) {
			LevelPiece* currPiece = pieces[h][w];
			this->assets->DrawLevelPieceMesh(*currPiece);
			currPiece->DebugDraw();
		}
	}
}

/*
 * Helper function for drawing the player paddle.
 */
void GameDisplay::DrawPlayerPaddle() {
	PlayerPaddle paddle = this->model->GetPlayerPaddle();
	this->assets->DrawPaddle(paddle);
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
	glColor3f(0, 1, 0);

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
}