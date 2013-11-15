/**
 * LevelCompleteSummaryDisplayState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __LEVELCOMPLETESUMMARYDISPLAYSTATE_H__
#define __LEVELCOMPLETESUMMARYDISPLAYSTATE_H__

#include "DisplayState.h"
#include "DecoratorOverlayPane.h"

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"

#include "../ESPEngine/ESP.h"

#include "../GameSound/SoundCommon.h"

class LevelCompleteSummaryDisplayState : public DisplayState {
public:
    LevelCompleteSummaryDisplayState(GameDisplay* display);
    ~LevelCompleteSummaryDisplayState();

    bool AllowsGameModelUpdates() const;
	void RenderFrame(double dT);

	void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton) { UNUSED_PARAMETER(pressedButton); }
    void MouseReleased(const GameControl::MouseButton& releasedButton) { UNUSED_PARAMETER(releasedButton); };

	void DisplaySizeChanged(int width, int height);

	DisplayState::DisplayStateType GetType() const;

private:
    static const double FADE_OUT_TIME;
    static const float FOOTER_VERTICAL_PADDING;
    static const float LEVEL_NAME_HORIZONTAL_PADDING;
    static const float HEADER_INBETWEEN_VERTICAL_PADDING;
    static const float TOTAL_SCORE_VALUE_INBETWEEN_HORIZONTAL_PADDING;
    static const float HEADER_SCORE_INBETWEEN_VERTICAL_PADDING;
    static const float SCORE_INBETWEEN_VERTICAL_PADDING;
    static const float FINAL_SCORE_INBETWEEN_VERTICAL_PADDING;
    static const float STAR_SIZE;
    static const float STAR_HORIZONTAL_GAP;
    static const float SCORE_LABEL_SIDE_PADDING;

    static const double POINTS_PER_SECOND;
    static const double MAX_TALLY_TIME_IN_SECS;
    static const double PER_SCORE_VALUE_FADE_IN_TIME;

    static const double SHOW_DIFFICULTY_CHOICE_PANE_TIME;
    static const double HIDE_DIFFICULTY_CHOICE_PANE_TIME;

    static const double POINT_SCORE_ANIM_TIME;

    bool waitingForKeyPress;
    AnimationLerp<double> scoreValueAnimation;
    AnimationLerp<float> fadeAnimation;
    AnimationLerp<float> levelCompleteTextScaleAnimation;
    AnimationLerp<float> newHighScoreFade;
    
    //AnimationLerp<float> maxBlocksFadeIn;
    //AnimationLerp<float> numItemsFadeIn;
    //AnimationLerp<float> totalTimeFadeIn;

    AnimationMultiLerp<Colour> footerColourAnimation;
    std::vector<AnimationLerp<float>* > starAnimations;
    AnimationLerp<float> totalScoreFlyInAnimation;
    AnimationLerp<float> totalScoreFadeInAnimation;
    
    int starAddAnimationCount;
    int currStarTotal;
    std::list<AnimationMultiLerp<float>*> starAddAlphaAnims;
    std::list<AnimationLerp<float>*> starAddMoveAnims;
    AnimationMultiLerp<Colour> starTotalColourAnim;
    AnimationMultiLerp<float> starTotalScaleAnim;

    ESPPointEmitter flareEmitter;

    ESPParticleRotateEffector flareRotator;
    ESPParticleScaleEffector haloGrower;
    ESPParticleColourEffector haloFader;

    TextLabel2D pressAnyKeyLabel;
    TextLabel2D levelCompleteLabel;
    TextLabel2D newHighScoreLabel;
    TextLabel2DFixedWidth* levelNameLabel;

    TextLabel2D starTotalLabel;

    // Statistics labels
    //TextLabel2DFixedWidth* maxBlocksTextLabel;
    //TextLabel2D maxBlocksValueLabel;
    //TextLabel2DFixedWidth* itemsAcquiredTextLabel;
    //TextLabel2D itemsAcquiredValueLabel;
    //TextLabel2DFixedWidth* levelTimeTextLabel;
    //TextLabel2D levelTimeValueLabel;

    TextLabel2D totalScoreLabel;
    TextLabel2D scoreValueLabel;

    float maxScoreValueWidth;
    float maxTotalLabelHeight;

    Texture* starTexture;
    Texture* glowTexture;
    Texture* sparkleTexture;

    Texture2D* lensFlareTex;
    Texture2D* haloTex;

    std::vector<ESPPointEmitter*> starBgEmitters;
    std::vector<ESPPointEmitter*> starFgEmitters;

    ESPParticleRotateEffector starBgRotator;
    ESPParticleRotateEffector starFgRotator;
    ESPParticleScaleEffector starFgPulser;

    SoundID bgLoopSoundID;
    SoundID highScoreSoundID;
    SoundID pointTallySoundID;

    class DifficultyPaneEventHandler : public OverlayPaneEventHandler {
    public:
        DifficultyPaneEventHandler(LevelCompleteSummaryDisplayState* state) : summaryState(state) {}
        LevelCompleteSummaryDisplayState* summaryState;
        void OptionSelected(const std::string& optionText);

    };
    DifficultyPaneEventHandler* difficultyChoiceHandler;
    DecoratorOverlayPane* difficultyChoicePane;

    void DrawLevelNameLabel(float currYPos, float screenWidth, float screenHeight);
    void DrawLevelCompleteLabel(float currYPos, float screenWidth, float screenHeight);
    void DrawStars(double dT, float currYPos, float screenWidth, float screenHeight);
    //void DrawMaxBlocksLabel(float currYPos, float screenWidth);
    //void DrawNumItemsLabel(float currYPos, float screenWidth);
    //void DrawTotalTimeLabel(float currYPos, float screenWidth);
    void DrawTotalScoreLabel(float currYPos, float screenWidth, float screenHeight);
    void DrawPressAnyKeyTextFooter(float screenWidth);
    void DrawStarTotalLabel(double dT, float screenWidth);

    void AnyKeyWasPressed();
        
    DISALLOW_COPY_AND_ASSIGN(LevelCompleteSummaryDisplayState);
};

inline bool LevelCompleteSummaryDisplayState::AllowsGameModelUpdates() const {
    return false;
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