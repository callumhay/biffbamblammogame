#ifndef __LEVELSTARTDISPLAYSTATE_H__
#define __LEVELSTARTDISPLAYSTATE_H__

#include "DisplayState.h"
#include "InGameRenderPipeline.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class GameDisplay;

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
	AnimationLerp<float> fadeInAnimation;	// Animation for fading in from white to the game

};

inline void LevelStartDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
}

inline void LevelStartDisplayState::KeyReleased(SDLKey key, SDLMod modifier) {
}

#endif // __LEVELSTARTDISPLAYSTATE_H__