/**
 * GameCompleteDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMECOMPLETEDISPLAYSTATE_H__
#define __GAMECOMPLETEDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

/**
 * State for displaying the completion of the game animation and
 * end credits.
 */
class GameCompleteDisplayState : public DisplayState {
public:
	GameCompleteDisplayState(GameDisplay* display);
	~GameCompleteDisplayState();

    bool AllowsGameModelUpdates() const { return true; }

	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);
	DisplayState::DisplayStateType GetType() const;


private:
	static const char* DEMO_COMPLETE_TITLE_TEXT;
    static const char* CONGRATS_TEXT;
    static const char* NOTE_TEXT;

    static const float TITLE_Y_GAP;
    static const float TITLE_CONGRATS_Y_GAP;
    static const float CONGRATS_NOTE_Y_GAP;
    static const float TEXT_LABEL_X_PADDING;

    static const float CAM_DIST_FROM_ORIGIN;

    static const double FADE_IN_TIME;
    static const double FADE_OUT_TIME;

	TextLabel2D titleLabel;
    TextLabel2DFixedWidth* congratsLabel;
    TextLabel2DFixedWidth* noteLabel;

    TextLabel2D creditLabel;
    TextLabel2D licenseLabel;

    Texture2D* starryBG;
    Texture2D* bbbLogoTex;

	float gameCompleteLabelRasterWidth;
   
    bool goToCredits;
    AnimationLerp<float> fadeAnim;

    DISALLOW_COPY_AND_ASSIGN(GameCompleteDisplayState);
};

inline void GameCompleteDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	UNUSED_PARAMETER(releasedButton);
}
inline void GameCompleteDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType GameCompleteDisplayState::GetType() const {
	return DisplayState::GameComplete;
}

#endif