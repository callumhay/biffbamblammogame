/**
 * LevelEndDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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

    bool AllowsGameModelUpdates() const { return false; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;

private:
    static const double RENDER_A_BIT_MORE_TIME;
	static const double FADE_TIME;

    double renderABitMoreCount;

	InGameRenderPipeline renderPipeline;
	AnimationLerp<float> fadeToWhiteAnimation;

};

inline void LevelEndDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                                const GameControl::ActionMagnitude& magnitude) {
	UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);
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