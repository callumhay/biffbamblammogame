/**
 * CreditsDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CREDITSDISPLAYSTATE_H__
#define __CREDITSDISPLAYSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "DisplayState.h"

class CreditsDisplayState : public DisplayState {
public:
	CreditsDisplayState(GameDisplay* display);
	~CreditsDisplayState();

    bool AllowsGameModelUpdates() const { return true; }
    
    void RenderFrame(double dT);

    void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
    void ButtonReleased(const GameControl::ActionButton& releasedButton) { UNUSED_PARAMETER(releasedButton); }
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); };
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

    void DisplaySizeChanged(int width, int height);
    DisplayState::DisplayStateType GetType() const;

private:
    class CreditLabel {
    public:
        CreditLabel(const char* title, const char* names, float maxWidth);
        CreditLabel(const TextureFontSet* titleFont, const char* title, const char* names, float maxWidth);
        ~CreditLabel();

        static void SetCreditsYVelocity(float yVel) { CreditLabel::creditsYVelocity = yVel; }

        bool GetIsNeverToBeVisibleAgain() const { return this->currState == NeverToBeVisibleAgain; }

        void StartFadeIn(double startTime, double endTime);
        void StartFadeOut(double startTime, double endTime);

        void SetYPos(float yPos);
        void Draw(CreditsDisplayState* state, double dT);

        float GetHeight() const;
        float GetTitleToNameGap() const;

    private:
        static const int TITLE_NAMES_V_GAP = 10;

        enum State { NotVisibleState, FadingInState, VisibleState, FadingOutState, NeverToBeVisibleAgain };

        State currState;
        float currYPos;
        AnimationLerp<float>  fadeAnim;
        TextLabel2DFixedWidth titleLabel;
        TextLabel2DFixedWidth namesLabel;

        static float creditsYVelocity;

        void SetState(State newState);
        void SetAlpha(float alpha);

        DISALLOW_COPY_AND_ASSIGN(CreditLabel);
    };

    static const int GAP_BETWEEN_CREDITS_LABELS = 75;

    static const double FADE_OUT_TIME_ON_BUTTON_PRESS;
    static const float CREDIT_SCROLLING_VELOCITY;

    static const char* CORE_CREDITS_1_TITLE_TEXT;
    static const char* CORE_CREDITS_1_NAMES_TEXT;
    static const char* CORE_CREDITS_2_TITLE_TEXT;
    static const char* CORE_CREDITS_2_NAMES_TEXT;
    static const char* CORE_CREDITS_SPECIAL_THANKS_TITLE_TEXT;
    static const char* CORE_CREDITS_SPECIAL_THANKS_NAMES_TEXT;

    static const char* FONT_CREDITS_CATEGORY_TITLE_TEXT;
    static const char* FONT_CREDITS_1_TITLE_TEXT;
    static const char* FONT_CREDITS_1_NAMES_TEXT;
    static const char* FONT_CREDITS_2_TITLE_TEXT;
    static const char* FONT_CREDITS_2_NAMES_TEXT;
    static const char* FONT_CREDITS_3_TITLE_TEXT;
    static const char* FONT_CREDITS_3_NAMES_TEXT;
    static const char* FONT_CREDITS_4_TITLE_TEXT;
    static const char* FONT_CREDITS_4_NAMES_TEXT;

    static const char* THIRD_PARTY_SW_CATEGORY_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_1_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_1_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_2_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_2_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_3_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_3_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_4_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_4_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_5_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_5_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_6_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_6_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_7_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_7_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_8_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_8_NAMES_TEXT;
    static const char* THIRD_PARTY_SW_9_TITLE_TEXT;
    static const char* THIRD_PARTY_SW_9_NAMES_TEXT;

    typedef std::list<CreditLabel*> CreditLabelContainer;
    typedef CreditLabelContainer::iterator CreditLabelContainerIter;

    Texture2D* bbbLogoTex;
    CreditLabelContainer creditLabels;
    AnimationLerp<float> logoFadeAnim;
    double countdownToScrollCredits;
    float bottomYCoordinateForFadeIn;
    float topYCoordinateForFadeOut;
    bool exitState;

    float GetBottomYCoordinateToStartFadingInAt() const;
    float GetTopYCoordinateToStartFadingOutAt() const;
    
    DISALLOW_COPY_AND_ASSIGN(CreditsDisplayState);
};

inline void CreditsDisplayState::DisplaySizeChanged(int width, int height) {
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType CreditsDisplayState::GetType() const {
    return DisplayState::Credits;
}

inline float CreditsDisplayState::GetBottomYCoordinateToStartFadingInAt() const {
    return this->bottomYCoordinateForFadeIn;
}

inline float CreditsDisplayState::GetTopYCoordinateToStartFadingOutAt() const {
    return this->topYCoordinateForFadeOut;
}

#endif // __CREDITSDISPLAYSTATE_H__
