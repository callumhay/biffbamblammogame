#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

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
	Mesh* testMesh;

	GameModel* model;
	GameAssets* assets;
	Camera* camera;
	GameEventsListener* gameListener;

	void DrawLevelPieces();
	void DrawPlayerPaddle();

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

	void ChangeDisplaySize(int w, int h);
	void Render();

	GameAssets* GetAssets() {
		return this->assets;
	}

};
#endif