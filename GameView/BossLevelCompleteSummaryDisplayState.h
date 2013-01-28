/**
 * BossLevelCompleteSummaryDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSLEVELCOMPLETESUMMARYDISPLAYSTATE_H__
#define __BOSSLEVELCOMPLETESUMMARYDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

class BossLevelCompleteSummaryDisplayState : public DisplayState {
public:
    BossLevelCompleteSummaryDisplayState(GameDisplay* display);
    ~BossLevelCompleteSummaryDisplayState();

    bool AllowsGameModelUpdates() const;
	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const double FADE_OUT_TIME;
    static const float FOOTER_VERTICAL_PADDING;
    static const float VICTORY_TO_WORLD_VERTICAL_PADDING;
    static const float WORLD_TO_COMPLETE_VERTICAL_PADDING;
    static const float VICTORY_LABEL_VERTICAL_SPACING;
    static const float LEFT_RIGHT_WORLD_LABEL_SPACING;
    static const float COMPLETE_TO_UNLOCKED_VERTICAL_PADDING;

    static const float MAX_COMPLETE_TEXT_SCALE;

    bool allAnimationIsDone;
    bool waitingForKeyPress;
    AnimationLerp<float> fadeInAnimation;
    AnimationLerp<float> fadeOutAnimation;
    AnimationMultiLerp<Colour> footerColourAnimation;

    AnimationMultiLerp<float> victoryLabelMoveAnim;
    AnimationLerp<float> worldCompleteAlphaAnim;
    AnimationLerp<float> unlockedAlphaAnim;

    AnimationLerp<float> glowRotationAnim;
    AnimationLerp<float> glowScaleAnim;

    TextLabel2D victoryLabel1;
    TextLabel2D victoryLabel2;
    TextLabel2D worldCompleteLabel;
    TextLabel2D* unlockedLabel;

    TextLabel2D pressAnyKeyLabel;

    Texture2D* bgTex;
    Texture2D* spinGlowTex;

    float GetCenterTextBlockTopYCoord(float screenHeight) const;
    float GetCenterTextBlockHeight() const;
    float GetVictoryLabelBlockHeight() const;
    float GetUnlockedLabelHeight() const;

    float DrawVictoryLabel(float screenWidth);
    float DrawWorldCompleteLabel(float screenWidth, float currYPos);
    void DrawUnlockedLabel(double dT, float screenWidth, float currYPos);
    void DrawPressAnyKeyTextFooter(float screenWidth);
    void AnyKeyWasPressed();

    DISALLOW_COPY_AND_ASSIGN(BossLevelCompleteSummaryDisplayState);
};

inline bool BossLevelCompleteSummaryDisplayState::AllowsGameModelUpdates() const {
    return false;
}

inline void BossLevelCompleteSummaryDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                                                const GameControl::ActionMagnitude& magnitude) {
    UNUSED_PARAMETER(pressedButton);
    UNUSED_PARAMETER(magnitude);
    this->AnyKeyWasPressed();
}

inline void BossLevelCompleteSummaryDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

inline void BossLevelCompleteSummaryDisplayState::MousePressed(const GameControl::MouseButton& pressedButton) {
    UNUSED_PARAMETER(pressedButton);
    this->AnyKeyWasPressed();
}

inline void BossLevelCompleteSummaryDisplayState::MouseReleased(const GameControl::MouseButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

inline void BossLevelCompleteSummaryDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType BossLevelCompleteSummaryDisplayState::GetType() const {
    return DisplayState::BossLevelCompleteSummary;
}


#endif // __BOSSLEVELCOMPLETESUMMARYDISPLAYSTATE_H__