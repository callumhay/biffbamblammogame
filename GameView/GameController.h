#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

class GameModel;
class GameDisplay;

/**
 * All key inputs from the user go through this class and
 * are processed and directed to the model and display from it.
 */
class GameController {

public:
	static const char SPACE_BAR_CHAR = ' ';
	static const char ENTER_CHAR = 13;
	static const char ESC_CHAR = 27;

private:	
	static const int NUM_KEYS = 256;

	GameModel* model;
	GameDisplay* display;

	bool keyPressed[NUM_KEYS];
	// Helper function for setting values in the keyPressed array.
	void SetKeyPress(int key, bool isPressed) {
		if (key < 0 || key >= NUM_KEYS) { return; }
		this->keyPressed[key] = isPressed;
	}

public:
	GameController(GameModel* model, GameDisplay* display);
	
	void ProcessNormalKeys(unsigned char key, int x, int y);
	void ProcessNormalKeysUp(unsigned char key, int x, int y);
	void ProcessSpecialKeys(int key, int x, int y);
	void ProcessSpecialKeysUp(int key, int x, int y);
	void ProcessMouse(int button, int state, int x, int y);
	void ProcessActiveMouseMotion(int x, int y);
	void ProcessPassiveMouseMotion(int x, int y);

	void Tick();
};

#endif