#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "DisplayState.h"
#include "../BlammoEngine/BlammoEngine.h"

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

private:
	Texture2D* renderToTexBackground;	// Texture of background in the game, rendered to texture
	Texture2D* renderToTexEverything;	// Texture of the entire game scene, rendered to texture

	// HUD related members
	static const std::string LIVES_LABEL_TEXT;
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;
	TextLabel2D scoreLabel;
	TextLabel2D livesLabel;
	
	void DrawGameHUD();
	void DrawGameScene(double dT);

	void DebugDrawLevelPieceBounds();
	void RenderBackgroundToFBO();
	void RenderFullSceneToFBO(double dT);
	void DrawScene(double dT);

public:
	InGameDisplayState(GameDisplay* display);
	virtual ~InGameDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(SDLKey key) {
		// Do nothing, key presses are interpretted completely by the game model,
		// while the game is being played.
	};
	virtual void DisplaySizeChanged(int width, int height);

};

#endif