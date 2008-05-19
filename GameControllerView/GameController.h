#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

class GameModel;

class GameController {
private:
	static const char SPACE_BAR_CHAR = ' ';
	static const char ESC_CHAR = 27;
	static const int NUM_KEYS = 256;

	GameModel* model;

	bool keyPressed[NUM_KEYS];
	// Helper function for setting values in the keyPressed array.
	void SetKeyPress(int key, bool isPressed) {
		if (key < 0 || key > NUM_KEYS) { return; }
		this->keyPressed[key] = isPressed;
	}

public:
	GameController(GameModel* model);
	
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