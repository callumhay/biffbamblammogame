#include "Utils/Includes.h"
#include "Utils/Algebra.h"

#include "GameControllerView/GameDisplay.h"
#include "GameControllerView/GameController.h"
#include "GameModel/GameModel.h"

static const char* WINDOW_TITLE = "Biff Bam Blammo";
static const int INIT_WIDTH = 1024;
static const int INIT_HEIGHT = 768;

static GameModel *model = NULL;
static GameController *controller = NULL;
static GameDisplay *display = NULL;


void ProcessNormalKeysMain(unsigned char key, int x, int y) {
	controller->ProcessNormalKeys(key, x, y);
}

void ProcessNormalKeysUpMain(unsigned char key, int x, int y) {
	controller->ProcessNormalKeysUp(key, x, y);
}

void ProcessSpecialKeysMain(int key, int x, int y) {
	controller->ProcessSpecialKeys(key, x, y);
}

void ProcessSpecialKeysUpMain(int key, int x, int y) {
	controller->ProcessSpecialKeysUp(key, x, y);
}

void ProcessMouseMain(int button, int state, int x, int y) {
	controller->ProcessMouse(button, state, x, y);
}

void ProcessActiveMouseMotionMain(int x, int y) {
	controller->ProcessActiveMouseMotion(x, y);
}

void ProcessPassiveMouseMotionMain(int x, int y) {
	controller->ProcessPassiveMouseMotion(x, y);
}

void ChangeSizeMain(int w, int h) {
	display->ChangeDisplaySize(w, h);
}

void RenderSceneMain() {
	display->Render();
	controller->Tick();
}

void FrameRenderMain(int value) {
	glutPostRedisplay();
	glutTimerFunc(GameDisplay::FRAME_DT_MILLISEC, FrameRenderMain, 1);
}

// Driver function for the game.
void main(int argc, char **argv) {
	Randomizer::InitializeRandomizer();

	// Set up the window using glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_ALPHA | GLUT_ACCUM | GLUT_STENCIL | GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(100,100); // TODO: make this the center of the screen if not fullscreen

	glutInitWindowSize(INIT_WIDTH,INIT_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	// Set the rendering functions for OGL
	glutDisplayFunc(RenderSceneMain);
	glutIdleFunc(RenderSceneMain);
	glutReshapeFunc(ChangeSizeMain);

	// Controller/input functions
	glutKeyboardUpFunc(ProcessNormalKeysUpMain);
	glutKeyboardFunc(ProcessNormalKeysMain);

	glutSpecialFunc(ProcessSpecialKeysMain);	
	glutSpecialUpFunc(ProcessSpecialKeysUpMain);

	glutMouseFunc(ProcessMouseMain);
	glutMotionFunc(ProcessActiveMouseMotionMain);
	glutPassiveMotionFunc(ProcessPassiveMouseMotionMain);

	model = new GameModel();
	display = new GameDisplay(model, INIT_WIDTH, INIT_HEIGHT);
	controller = new GameController(model);

	// Render loop...
	glutTimerFunc(GameDisplay::FRAME_DT_MILLISEC, FrameRenderMain, 1);
	glutMainLoop();

	delete model;
	delete display;
	delete controller;
}