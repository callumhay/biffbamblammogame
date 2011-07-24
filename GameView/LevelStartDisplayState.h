/**
 * LevelStartDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LEVELSTARTDISPLAYSTATE_H__
#define __LEVELSTARTDISPLAYSTATE_H__

#include "DisplayState.h"
#include "InGameRenderPipeline.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class GameDisplay;
class ESPPointEmitter;

class LevelStartDisplayState : public DisplayState {
public:
	LevelStartDisplayState(GameDisplay* display);
	~LevelStartDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
	InGameRenderPipeline renderPipeline;

	static const double FADE_IN_TIME;
	static const double WIPE_TIME;
	static const double LEVEL_TEXT_FADE_OUT_TIME;
	static const double LEVEL_BLOCK_FADE_IN_TIME;

	static const float LEVEL_NAME_WIPE_FADE_QUAD_SIZE;
	static const float LEVEL_TEXT_X_PADDING;
	static const float LEVEL_TEXT_Y_PADDING;

	TextLabel2D levelNameLabel;
	AnimationLerp<float> fadeInAnimation;							// Animation for fading in from white to the game
	AnimationLerp<float> showLevelNameWipeAnimation;	// Animation for wipe-fadein of the level name label
	AnimationLerp<float> levelNameFadeOutAnimation;		// Animation to fade out the level text
	AnimationLerp<float> dropShadowAnimation;					// Animation for level text drop shadow
	AnimationLerp<float> blockFadeInAnimation;				// Animation for fading in the level's blocks

	AnimationLerp<float> paddleMoveUpAnimation;				// Movement from the bottom of the screen into position
	AnimationLerp<float> ballFadeInAnimation;					// Animation for ball fade in

	// Emitters for when the ball appears
	ESPPointEmitter* shockwaveEmitter;
	ESPPointEmitter* starEmitter;

};

inline void LevelStartDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
}

inline void LevelStartDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline DisplayState::DisplayStateType LevelStartDisplayState::GetType() const {
	return DisplayState::LevelStart;
}

#endif // __LEVELSTARTDISPLAYSTATE_H__