/**
 * BallBoostHUD.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __BALLBOOSTHUD_H__
#define __BALLBOOSTHUD_H__

#include "LivesLeftHUD.h"
#include "../BlammoEngine/Animation.h"

#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"

#include "../GameModel/BallBoostModel.h"

class Texture;
class Texture2D;
class ESPPointEmitter;
class Camera;

class BallBoostHUD {
public:
    static const int LIVES_BOOST_GAP  = 10;

    static const int H_BORDER_SPACING = LivesLeftHUD::BORDER_SPACING;
    static const int V_BORDER_SPACING = LivesLeftHUD::BORDER_SPACING + LivesLeftHUD::ELEMENT_SIZE + LIVES_BOOST_GAP;

    static const int BALL_BOOST_HUD_HEIGHT = 42;
    static const int BALL_BOOST_HUD_WIDTH  = 4 * BALL_BOOST_HUD_HEIGHT;
    static const int BALL_BOOST_HUD_HALF_HEIGHT = BALL_BOOST_HUD_HEIGHT / 2;
    static const int BALL_BOOST_HUD_HALF_WIDTH  = BALL_BOOST_HUD_WIDTH  / 2;

    BallBoostHUD(int displayHeight);
    ~BallBoostHUD();

    const Colour& GetCurrentBoostColour() const;

    void Draw(const Camera& camera, const BallBoostModel* model, int displayHeight, double dT);
    void BoostGained();
    void BoostLost();
    void AllBoostsLost(const BallBoostModel* model);
    void Reinitialize();

    void SetAlpha(float alpha);

private:
    static const float BALL_FILL_START_PERCENT;
    static const float BALL_FILL_END_PERCENT;
    static const float BALL_FILL_DIFF_PERCENT;

    float alpha;

    Texture* ballHUDOutlineTex;
    Texture* ballFillTex;
    
	ESPParticleScaleEffector haloExpander;
	ESPParticleColourEffector haloFader;
	Texture2D* haloTexture;
    ESPPointEmitter* boostGainedHaloEmitter;
    std::list<ESPPointEmitter*> activeEffects;

    AnimationLerp<float> tempFillLostAnim;

    // Private class for representing each of the fills on the tail of the boost HUD.
    // Intended to encapsulate more complicated animations and effects associated with each tail.
    class TrailFill {
    public:
        static const double FILL_TIME_IN_SECONDS;
        static const double UNFILL_TIME_IN_SECONDS;

        TrailFill(const char* trailTexFilepath, const ColourRGBA& colour);
        ~TrailFill();
        
        void Draw(double dT);

        void Gained();
        void Lost(double startTime = 0.0);
        void Clear();

        float GetCurrWidth() const { return this->widthAnim.GetInterpolantValue(); }
        void SetAlpha(float alpha) { this->colour[3] = alpha; }
        const Colour& GetColour() const { return this->colour.GetColour(); }

    private:
        Texture* trailTex;
        ColourRGBA colour;
        AnimationLerp<float> widthAnim;

        DISALLOW_COPY_AND_ASSIGN(TrailFill);
    };

    std::vector<TrailFill*> trailFills;
    int currNumTrailFills;

    BallBoostHUD::TrailFill* GetCurrentTrailFill() const;

    DISALLOW_COPY_AND_ASSIGN(BallBoostHUD);
};

inline const Colour& BallBoostHUD::GetCurrentBoostColour() const {
    return this->GetCurrentTrailFill()->GetColour();
}

inline BallBoostHUD::TrailFill* BallBoostHUD::GetCurrentTrailFill() const {
    return this->trailFills[std::min<int>(BallBoostModel::TOTAL_NUM_BOOSTS-1, this->currNumTrailFills)];
}

#endif // __BALLBOOSTHUD_H__