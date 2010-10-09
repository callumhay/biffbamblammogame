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

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);

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

inline void LevelStartDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
}

inline void LevelStartDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

#endif // __LEVELSTARTDISPLAYSTATE_H__