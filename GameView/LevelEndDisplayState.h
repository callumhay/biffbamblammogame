#ifndef __LEVELENDDISPLAYSTATE_H__
#define __LEVELENDDISPLAYSTATE_H__

#include "DisplayState.h"
#include "InGameRenderPipeline.h"

#include "../BlammoEngine/Animation.h"

class GameDisplay;

class LevelEndDisplayState : public DisplayState {
public:
	LevelEndDisplayState(GameDisplay* display);
	~LevelEndDisplayState();

	void RenderFrame(double dT);
	void KeyPressed(SDLKey key, SDLMod modifier);
	void KeyReleased(SDLKey key, SDLMod modifier);
	void DisplaySizeChanged(int width, int height);

private:
	static const double FADE_TIME;

	InGameRenderPipeline renderPipeline;
	AnimationLerp<float> fadeToWhiteAnimation;

};

inline void LevelEndDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
	UNUSED_PARAMETER(key);
	UNUSED_PARAMETER(modifier);
}

inline void LevelEndDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
	UNUSED_PARAMETER(key);
	UNUSED_PARAMETER(modifier);
}

inline void LevelEndDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
	assert(false);
}

#endif // __LEVELENDDISPLAYSTATE_H__