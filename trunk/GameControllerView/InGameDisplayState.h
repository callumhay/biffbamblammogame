#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include <sstream>
#include "DisplayState.h"
#include "TextLabel.h"


/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

private:
	// HUD related members
	static const std::string LIVES_LABEL_TEXT;
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;
	TextLabel2D scoreLabel;
	TextLabel2D livesLabel;
	

	// Perspective values for the game projection
	static const float FOV_ANGLE_IN_DEGS;
	static const float NEAR_PLANE_DIST;
	static const float FAR_PLANE_DIST;

	void DrawLevelPieces();
	void DrawPlayerPaddle();

public:
	InGameDisplayState(GameDisplay* display);
	virtual ~InGameDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key) {
		// Do nothing, key presses are interpretted completely by the game model,
		// while the game is being played.
	};
	
};

#endif