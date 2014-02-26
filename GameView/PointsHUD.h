/**
 * PointsHUD.h
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

#ifndef __POINTSHUD_H__
#define __POINTSHUD_H__

#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

#include "../ESPEngine/ESPParticleRotateEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

#include "../GameModel/GameLevel.h"


class Camera;
class Texture;
class PointAward;
class ESPPointEmitter;

/**
 * Class for encapsulating all animations and display for the Points Heads-up-display
 * which displays the current point total of the player, their current multiplier, number of
 * stars and a running display of any currently obtained point bonuses / scores.
 */
class PointsHUD {
public:
    static const int SCREEN_EDGE_VERTICAL_GAP;
    static const int SCREEN_EDGE_HORIZONTAL_GAP;

    PointsHUD();
    ~PointsHUD();

    //enum AnimationType { CreationAnimation, IdleAnimation };

    void Draw(const Camera& camera, int displayWidth, int displayHeight, double dT);

    void Reinitialize();

    void DoImmediatePointAnimation();

    void SetNumStars(const Camera& camera, int numStars);
    void SetScore(long pointScore);
    void SetMultiplierCounter(int count);
    void SetMultiplier(int multiplierAmt);
    //void PostPointNotification(const PointAward& pointAward);
    
    void SetAlpha(float alpha);

    //static const char* GetPointNotificationName(const PointAward& pointAward);

private:
    static const int STAR_SIZE;
    static const float STAR_HALF_SIZE;
    static const float STAR_ALPHA;
    static const int STAR_GAP;
    static const int STAR_TO_SCORE_VERTICAL_GAP;
    static const int SCORE_TO_MULTIPLER_HORIZONTAL_GAP;
    static const int ALL_STARS_WIDTH;

    class MultiplierHUD {
    public:
        MultiplierHUD();
        ~MultiplierHUD();

        void Reinitialize();
        
        void SetAlpha(float alpha);
        void SetMultiplier(int multiplierAmt);
        void Draw(float rightMostX, float topMostY, double dT);

    private:
        static const float BANG_TEXT_BORDER;
        

        enum AnimationState { None, CreationAnimation, IncreaseAnimation, IdleAnimation, LeaveAnimation };
        AnimationState currState;

        float size;                     // The total size (width/height) of the multiplier
        int currPtMultiplier;           // The current score multiplier
        
        TextLabel2D* ptMultiplierLabel; // The display label for the multiplier
        Texture* multiplierBangTex;     // The 'bang' texture behind the multiplier label

        // Animations
        AnimationMultiLerp<float> scaleAnim;
        AnimationMultiLerp<ColourRGBA> rgbaLabelAnim;
        
        void DrawHUD(float rightMostX, float topMostY);
        void SetCurrentAnimationState(const AnimationState& state);

        static float GetMultiplierScale(int multiplier);

        DISALLOW_COPY_AND_ASSIGN(MultiplierHUD);
    };

    class MultiplierGageHUD {
    public:
        static const int MULTIPLIER_GAGE_SIZE;
        static const int HALF_MULTIPLIER_GAGE_SIZE;

        MultiplierGageHUD();
        ~MultiplierGageHUD();

        void Reinitialize();

        void SetAlpha(float alpha);
        void SetMultiplierCounterAmount(int multiplierCntAmt);
        void Draw(float rightMostX, float topMostY, double dT);

    private:
        float alpha;

        int currMultiplierCounterIdx;
        Texture* multiplierGaugeOutlineTex;                 // The outline for the multiplier gage
        Texture* multiplierGaugeFillTex;
        std::vector<Texture*> multiplierGageFillTexs;      // Fills for the multiplier gage (there are 9 of them)

        DISALLOW_COPY_AND_ASSIGN(MultiplierGageHUD);
    };

    int numStars;                      // The current number of stars awarded to the player
    long currPtScore;                  // The current score
    MultiplierHUD* multiplier;         // HUD for the multiplier
    MultiplierGageHUD* multiplierGage; // HUD for the gauge that determines the multiplier
    
    // Labels
    TextLabel2D* ptScoreLabel;
    
    // Animations
    AnimationLerp<long> scoreAnimator;   // Animates the current score (so that it looks like it's tallying points over time)
  
    std::vector<AnimationMultiLerp<float>*> starSizeAnimators;
    std::vector<AnimationMultiLerp<ColourRGBA>*> starColourAnimators;
    AnimationMultiLerp<ColourRGBA> flashingStarAnimator;

    // Textures
    Texture* starTex;
    Texture* lensFlareTex;
    Texture* haloTex;

    // Emitters
    ESPParticleRotateEffector flareRotator;
    ESPParticleScaleEffector haloGrower;
    ESPParticleColourEffector haloFader;
    std::list<ESPPointEmitter*> starEffectEmitters;

    float starAlpha;

    Point2D GetStarStartPos(float displayWidth) const;

    void DrawMultiplier(float rightMostX, float topMostY);
    void DrawIdleStars(const Camera& camera, float rightMostX, float topMostY, double dT);
    void DrawQuad(float centerX, float centerY, float size);

    void SetStarAcquiredAnimation(const Camera& camera, size_t starIdx);

    DISALLOW_COPY_AND_ASSIGN(PointsHUD);
};

inline void PointsHUD::DoImmediatePointAnimation() {
    this->scoreAnimator.SetInterpolantValue(this->scoreAnimator.GetTargetValue());
}

inline void PointsHUD::SetNumStars(const Camera& camera, int numStars) {
    assert(numStars >= 0);
    assert(numStars <= GameLevel::MAX_STARS_PER_LEVEL);

    // Setup the animations for any acquired stars
    int starDiff = numStars - this->numStars;
    if (starDiff > 0) {
        for (int i = 0; i < starDiff; i++) {
            this->SetStarAcquiredAnimation(camera, this->numStars + i);
        }
    }

    this->numStars = numStars;
}

inline void PointsHUD::SetScore(long pointScore) {

    // This will animated and update the score as calls to Draw are made
    if (this->scoreAnimator.GetTargetValue() != pointScore) {
        this->scoreAnimator.SetLerp(0.25, pointScore);
    }
}

inline void PointsHUD::SetMultiplierCounter(int count) {
    this->multiplierGage->SetMultiplierCounterAmount(count);
}

inline void PointsHUD::SetMultiplier(int multiplierAmt) {
    assert(multiplierAmt >= 1);
    this->multiplier->SetMultiplier(multiplierAmt);
}

inline void PointsHUD::MultiplierGageHUD::SetMultiplierCounterAmount(int multiplierCntAmt) {
    assert(multiplierCntAmt >= 0);
    this->currMultiplierCounterIdx = std::min<int>(static_cast<int>(this->multiplierGageFillTexs.size()), multiplierCntAmt) - 1;
}

inline Point2D PointsHUD::GetStarStartPos(float displayWidth) const {
    return Point2D(displayWidth - SCREEN_EDGE_HORIZONTAL_GAP - 
        MultiplierGageHUD::MULTIPLIER_GAGE_SIZE, this->ptScoreLabel->GetTopLeftCorner()[1] - 
        this->ptScoreLabel->GetHeight() - 3);
}

inline void PointsHUD::MultiplierGageHUD::SetAlpha(float alpha) {
    this->alpha = alpha;
}


#endif // __POINTSHUD_H__