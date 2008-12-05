#ifndef __INGAMEDISPLAYSTATE_H__
#define __INGAMEDISPLAYSTATE_H__

#include "DisplayState.h"
#include "../BlammoEngine/BlammoEngine.h"

// TODO: get rid of this?
#include "../ESPEngine/ESP.h"

/**
 * State for representing and rendering the display of the game as it is
 * being played by the user.
 */
class InGameDisplayState : public DisplayState {

private:
	Texture2D* renderToTexBeforeBall;	// Texture ID of the game scene rendered to texture before drawing the ball

	// HUD related members
	static const std::string LIVES_LABEL_TEXT;
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;
	TextLabel2D scoreLabel;
	TextLabel2D livesLabel;
	
	void DrawGameHUD();
	void DrawGameScene(double dT);
	
	ESPPointEmitter ptEmitTest;
	ESPParticleColourEffector colourEffector;
	ESPParticleVelocityEffector mvEffector;

public:
	InGameDisplayState(GameDisplay* display);
	virtual ~InGameDisplayState();

	virtual void RenderFrame(double dT);
	virtual void KeyPressed(unsigned char key) {
		// Do nothing, key presses are interpretted completely by the game model,
		// while the game is being played.
	};
	virtual void DisplaySizeChanged(int width, int height);

};

#endif