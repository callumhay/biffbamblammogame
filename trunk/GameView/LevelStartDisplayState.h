/**
 * LevelStartDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LEVELSTARTDISPLAYSTATE_H__
#define __LEVELSTARTDISPLAYSTATE_H__

#include "DisplayState.h"
#include "InGameRenderPipeline.h"
#include "GameDisplay.h"
#include "MouseRenderer.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"
#include "../GameSound/SoundCommon.h"

class GameDisplay;
class ESPPointEmitter;

class LevelStartDisplayState : public DisplayState {
public:
	LevelStartDisplayState(GameDisplay* display);
	~LevelStartDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };
    void MouseMoved(int oglWindowCoordX, int oglWindowCoordY);
    void WindowFocus(bool windowHasFocus);
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

    // Sound IDs for tracking level starting sounds
    SoundID paddleMoveUpSoundID;
    SoundID ballSpawnSoundID;

    DISALLOW_COPY_AND_ASSIGN(LevelStartDisplayState);
};

inline void LevelStartDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                                  const GameControl::ActionMagnitude& magnitude) {
	UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);
}

inline void LevelStartDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}

inline void LevelStartDisplayState::MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
    this->display->GetMouse()->ShowMouse(oglWindowCoordX, oglWindowCoordY);
}

inline void LevelStartDisplayState::WindowFocus(bool windowHasFocus) {
    this->display->GetMouse()->SetWindowHasFocus(windowHasFocus);
}

inline DisplayState::DisplayStateType LevelStartDisplayState::GetType() const {
	return DisplayState::LevelStart;
}

#endif // __LEVELSTARTDISPLAYSTATE_H__