#ifndef __WORLDSTARTDISPLAYSTATE_H__
#define __WORLDSTARTDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class GameDisplay;

/**
 * The world start display state is a splash screen
 * in order to inform the user of the name of the level / where they are in the game.
 * It then fades to white once the user presses any key and starts the level.
 */
class WorldStartDisplayState : public DisplayState {

public:
	WorldStartDisplayState(GameDisplay* display);
	~WorldStartDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);

private:
	static const double FADE_OUT_TIME;
	static const double HEADER_WIPE_TIME;
	static const double FOOTER_FLASH_TIME;
	static const float  HEADER_WIPE_FADE_QUAD_SIZE;
	float minMaxLineWidth[2];

	TextLabel2D worldNameLabel;
	TextLabel2D nowEnteringLabel;
	TextLabel2D pressAnyKeyLabel;

	bool waitingForKeyPress;
	
	AnimationLerp<float> fadeAnimation;							// Animation for fading to white - represents alpha fade value
	AnimationLerp<float> showHeaderAnimation;				// Animation for wipe-fadein of the header
	AnimationLerp<float> titleFadeIn;
	AnimationLerp<float> titleMoveIn;
	AnimationMultiLerp<Colour> footerColourAnimation;

	void DrawNowEnteringTextHeader(float screenWidth, float screenHeight);
	void DrawPressAnyKeyTextFooter(float screenWidth, float screenHeight);

};

inline void WorldStartDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}


#endif // __WORLDSTARTDISPLAYSTATE_H__