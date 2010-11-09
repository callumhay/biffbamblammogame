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
	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
	static const double FADE_TIME;

	InGameRenderPipeline renderPipeline;
	AnimationLerp<float> fadeToWhiteAnimation;

};

inline void LevelEndDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	UNUSED_PARAMETER(pressedButton);
}

inline void LevelEndDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline void LevelEndDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
	assert(false);
}

inline DisplayState::DisplayStateType LevelEndDisplayState::GetType() const {
	return DisplayState::LevelEnd;
}

#endif // __LEVELENDDISPLAYSTATE_H__