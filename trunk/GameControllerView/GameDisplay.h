#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

#include "../Utils/Debug.h"
#include "DisplayState.h"
#include "Camera.h"

class GameModel;
class Mesh;
class GameAssets;
class Point2D;
class GameEventsListener;

// The main display class, used to execute the main rendering loop
// and adjust size, etc.
class GameDisplay {

private:
	Camera gameCamera;

	DisplayState* currState;
	GameModel* model;
	GameAssets* assets;
	GameEventsListener* gameListener;

	// Width and height of the viewport/display
	int width, height;
	
	// Functions for Action Listeners
	void SetupActionListeners();
	void RemoveActionListeners();

	// Debug draw stuff
	static void DrawDebugAxes();
	static void DrawDebugUnitGrid(bool xy, bool xz, bool zy, int numGridTicks);

public:
	static const int FRAMERATE = 60;
	static const double FRAME_DT_MILLISEC;
	static const double FRAME_DT_SEC;	// Delta time between frames

	GameDisplay(GameModel* model, int initWidth, int initHeight);
	~GameDisplay();

	// Change the current state of the display
	void SetCurrentState(DisplayState* state) {
		assert(state != NULL);
		if (this->currState != NULL) {
			delete this->currState;
		}
		this->currState = state;
	}

	void ChangeDisplaySize(int w, int h);
	void Render();

	// Functions for setting up different types of render options
	static void SetInitialRenderOptions();
	static void SetOutlineRenderAttribs(float outlineWidth = 1.0f);

	// Getters and Setters
	GameAssets* GetAssets() {
		return this->assets;
	}
	GameModel* GetModel() {
		return this->model;
	}
	int GetDisplayWidth() const {
		return this->width;
	}
	int GetDisplayHeight() const {
		return this->height;
	}
	Camera& GetCamera() {
		return this->gameCamera;
	}

	// Tells the display that a certain key was pressed
	void KeyPressed(unsigned char key) {
		this->currState->KeyPressed(key);
	}

	// Be friends with the state classes
	//friend class InGameDisplayState;

};
#endif