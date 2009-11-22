#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Camera.h"
#include "DisplayState.h"

class GameModel;
class GameAssets;
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
	
	bool gameReinitialized;		// Whether or not we should reinitialize the whole game (recreate the window, etc.)
	bool gameExited;  				// Whether or not the game has been exited

	// Functions for Action Listeners
	void SetupActionListeners();
	void RemoveActionListeners();

#ifdef _DEBUG
	static bool drawDebugBounds;
	static bool drawDebugLightGeometry;
	static bool detachedCamera;
	// Debug draw stuff
	void DrawDebugAxes();
	void DrawDebugUnitGrid(bool xy, bool xz, bool zy, int numGridTicks);
#endif

public:
	static const int MAX_FRAMERATE;							// The maximum framerate possible for the game				
	static const unsigned long FRAME_SLEEP_MS;	// Time to sleep between frames (determined by MAX_FRAMERATE)

	GameDisplay(GameModel* model, int initWidth, int initHeight);
	~GameDisplay();

	bool HasGameExited() const { return this->gameExited; }
	bool ShouldGameReinitialize() const { return this->gameReinitialized; }

	void QuitGame() { this->gameExited = true; }
	void ReinitializeGame() { this->gameReinitialized = true; }

	// Change the current state of the display
	void SetCurrentState(DisplayState* state) {
		assert(state != NULL);
		if (this->currState != NULL) {
			delete this->currState;
		}
		this->currState = state;
	}

	void ChangeDisplaySize(int w, int h);
	void Render(double dT);

	// Functions for setting up different types of render options
	static void SetInitialRenderOptions();

	// Getters and Setters
	GameAssets* GetAssets() {
		return this->assets;
	}
	GameModel* GetModel() {
		return this->model;
	}
	Camera& GetCamera() {
		return this->gameCamera;
	}

	// Tells the display that a certain key was pressed
	void KeyPressed(SDLKey key) {
		this->currState->KeyPressed(key);
	}

#ifdef _DEBUG

	static void ToggleDrawDebugBounds() {
		drawDebugBounds = !drawDebugBounds;
	}
	static bool IsDrawDebugBoundsOn() {
		return drawDebugBounds;
	}
	static void ToggleDrawDebugLightGeometry() {
		drawDebugLightGeometry = !drawDebugLightGeometry;
	}
	static bool IsDrawDebugLightGeometryOn() {
		return drawDebugLightGeometry;
	}
	static void ToggleDetachCamera() {
		detachedCamera = !detachedCamera;
	}
	static bool IsCameraDetached() {
		return detachedCamera;
	}
#endif

	// Be friends with the state classes
	//friend class InGameDisplayState;

};
#endif