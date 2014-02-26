/**
 * BossLevelCompleteSummaryDisplayState.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOSSLEVELCOMPLETESUMMARYDISPLAYSTATE_H__
#define __BOSSLEVELCOMPLETESUMMARYDISPLAYSTATE_H__

#include "DisplayState.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameSound/SoundCommon.h"

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
    static const double FADE_TIME;
    static const float FOOTER_VERTICAL_PADDING;
    static const float VICTORY_TO_WORLD_VERTICAL_PADDING;
    static const float WORLD_TO_COMPLETE_VERTICAL_PADDING;
    static const float VICTORY_LABEL_VERTICAL_SPACING;
    static const float LEFT_RIGHT_WORLD_LABEL_SPACING;
    static const float COMPLETE_TO_UNLOCKED_VERTICAL_PADDING;

    static const float MAX_COMPLETE_TEXT_SCALE;

    bool allAnimationIsDone;
    bool waitingForKeyPress;
    bool leavingState;

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
    TextLabel2D* worldCompleteLabel;
    TextLabel2D* unlockedLabel;

    TextLabel2D pressAnyKeyLabel;

    Texture2D* spinGlowTex;
    Texture2D* flareTex;
    Texture2D* bangStarTex;

    SoundID bgMusicSoundID;
    SoundID victoryMessageSoundID;

    float GetCenterTextBlockTopYCoord(float screenHeight) const;
    float GetCenterTextBlockHeight() const;
    float GetVictoryLabelBlockHeight() const;
    float GetWorldCompleteLabelHeight() const;
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