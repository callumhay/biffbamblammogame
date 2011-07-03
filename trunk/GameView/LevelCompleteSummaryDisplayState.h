/**
 * LevelCompleteSummaryDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LEVELCOMPLETESUMMARYDISPLAYSTATE_H__
#define __LEVELCOMPLETESUMMARYDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class LevelCompleteSummaryDisplayState : public DisplayState {
public:
    LevelCompleteSummaryDisplayState(GameDisplay* display);
    ~LevelCompleteSummaryDisplayState();

    bool AllowsGameModelUpdates() const;
	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const double FADE_OUT_TIME;
    static const double FOOTER_FLASH_TIME;

    bool waitingForKeyPress;
    AnimationLerp<float> fadeAnimation;
    AnimationMultiLerp<Colour> footerColourAnimation;

    TextLabel2D pressAnyKeyLabel;

    void DrawPressAnyKeyTextFooter(float screenWidth);

    DISALLOW_COPY_AND_ASSIGN(LevelCompleteSummaryDisplayState);
};

inline bool LevelCompleteSummaryDisplayState::AllowsGameModelUpdates() const {
    return false;
}

inline void LevelCompleteSummaryDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    UNUSED_PARAMETER(pressedButton);
	if (this->waitingForKeyPress) {
		// Start the fade out animation - the user wants to start playing!
		this->fadeAnimation.SetLerp(LevelCompleteSummaryDisplayState::FADE_OUT_TIME, 1.0f);
        waitingForKeyPress = false;
	}
}

inline void LevelCompleteSummaryDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

inline void LevelCompleteSummaryDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType LevelCompleteSummaryDisplayState::GetType() const {
    return DisplayState::LevelCompleteSummary;
}

#endif // __LEVELCOMPLETESUMMARYDISPLAYSTATE_H__