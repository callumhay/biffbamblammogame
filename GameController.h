#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class GameDisplay;

/**
 * All key inputs from the user go through this class and
 * are processed and directed to the model and display from it.
 */
class GameController {

private:	
	static const int NUM_KEYS = SDLK_LAST;

	GameModel* model;
	GameDisplay* display;

	bool keyPressed[NUM_KEYS];
	
	// Helper function for setting values in the keyPressed array.
	void SetKeyPress(int key, bool isPressed) {
		if (key < 0 || key >= NUM_KEYS) { return; }
		this->keyPressed[key] = isPressed;
	}

public:
#ifdef _DEBUG
	// TODO: seperate this to debug functionality...
	static float DEBUG_highlightThreshold;
	static float DEBUG_sceneIntensity;
	static float DEBUG_glowIntensity;
	static float DEBUG_highlightIntensity;
#endif

	GameController(GameModel* model, GameDisplay* display);
	
	void KeyDown(SDLKey key);
	void KeyUp(SDLKey key);

	void Tick();
};

#endif