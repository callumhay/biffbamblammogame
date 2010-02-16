#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/FBObj.h"

#include "DisplayState.h"

class Texture2D;

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

	void DrawGameHUD(double dT);
	void DrawGameScene(double dT);

	void RenderBackgroundToFBO(double dT);
	void RenderForegroundWithBackgroundToFBO(double dT);
	void RenderFinalGather(double dT);

#ifdef _DEBUG
	void DebugDrawBounds();
#endif

public:
	InGameDisplayState(GameDisplay* display);
	~InGameDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key);
	void DisplaySizeChanged(int width, int height);

};

#endif