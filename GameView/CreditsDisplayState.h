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
        ~CreditLabel();

        void SetAlpha(float alpha);
        void SetYPos(float y);

        void Draw();

        float GetHeight() const;
        float GetTitleToNameGap() const;

    private:
        static const int TITLE_NAMES_V_GAP = 10;

        TextLabel2DFixedWidth titleLabel;
        TextLabel2DFixedWidth namesLabel;

        DISALLOW_COPY_AND_ASSIGN(CreditLabel);
    };

    static const int GAP_BETWEEN_CREDITS_LABELS = 75;

    static const char* CREDITS_1_TITLE_TEXT;
    static const char* CREDITS_1_NAMES_TEXT;
    static const char* CREDITS_2_TITLE_TEXT;
    static const char* CREDITS_2_NAMES_TEXT;
    static const char* SPECIAL_THANKS_TITLE_TEXT;
    static const char* SPECIAL_THANKS_NAMES_TEXT;

    Texture2D* bbbLogoTex;

    CreditLabel creditLabel1;
    CreditLabel creditLabel2;
    CreditLabel specialThanksLabel;

    AnimationLerp<float> logoFadeinAnim;
    AnimationLerp<float> fadeInCreditLabel1Anim;
    AnimationLerp<float> fadeInCreditLabel2Anim;
    AnimationLerp<float> fadeInSpecialThanksAnim;

    AnimationLerp<float> fadeoutAnim;

    bool exitState;
    
    DISALLOW_COPY_AND_ASSIGN(CreditsDisplayState);
};

inline void CreditsDisplayState::DisplaySizeChanged(int width, int height) {
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(height);
}

inline DisplayState::DisplayStateType CreditsDisplayState::GetType() const {
    return DisplayState::Credits;
}

#endif // __CREDITSDISPLAYSTATE_H__
