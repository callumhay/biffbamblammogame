#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "DisplayState.h"

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

private:
	// Perspective values for the game projection
	static const float FOV_ANGLE_IN_DEGS;
	static const float NEAR_PLANE_DIST;
	static const float FAR_PLANE_DIST;

	void DrawLevelPieces();
	void DrawPlayerPaddle();

public:
	InGameDisplayState(GameDisplay* display);
	virtual ~InGameDisplayState();

	virtual void RenderFrame();
	virtual void KeyPressed(unsigned char key) {
		// Do nothing, key presses are interpretted completely by the game model,
		// while the game is being played.
	};
	
};

#endif