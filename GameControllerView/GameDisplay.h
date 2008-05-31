#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

#include "../Utils/Debug.h"

#include "DisplayState.h"

class GameModel;
class Mesh;
class GameAssets;
class Camera;
class Point2D;
class GameEventsListener;

// The main display class, used to execute the main rendering loop
// and adjust size, etc.
class GameDisplay {

private:
	DisplayState* currState;
	GameModel* model;
	GameAssets* assets;
	GameEventsListener* gameListener;

	int width, height;

	// Render setup
	void SetupRenderOptions();
	
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

	// Tells the display that a certain key was pressed
	void KeyPressed(unsigned char key) {
		this->currState->KeyPressed(key);
	}

	// Be friends with the state classes
	//friend class InGameDisplayState;

};
#endif