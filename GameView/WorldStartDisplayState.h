/**
 * WorldStartDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

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
	void DrawPressAnyKeyTextFooter(float screenWidth);

	DISALLOW_COPY_AND_ASSIGN(WorldStartDisplayState);
};

inline void WorldStartDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline DisplayState::DisplayStateType WorldStartDisplayState::GetType() const {
	return DisplayState::WorldStart;
}

#endif // __WORLDSTARTDISPLAYSTATE_H__