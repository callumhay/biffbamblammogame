/**
 * BBBTitleDisplay.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BBBTITLEDISPLAY_H__
#define __BBBTITLEDISPLAY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"

#include "../ESPEngine/ESP.h"

#include "CgFxPostRefract.h"

class Texture2D;
class Camera;
class GameSound;

/**
 * Class for displaying the emitters required to show the
 * "Biff! Bam!! Blammo!?!" title and any animations / sound associated with it.
 */
class BBBTitleDisplay {
public:
    BBBTitleDisplay(float scaleFactor, double fadeInTime, GameSound* sound);
    ~BBBTitleDisplay();

    void SetAlpha(float alpha);
    bool IsFinishedAnimating() const;

    void Draw(double dT, Camera& camera, const Texture2D* fboTex);

private:
    float scaleFactor;
    float alpha;

    GameSound* sound;     // Pointer to the sound module, not owned by this

    Texture2D* biffTex;
    Texture2D* bamTex;
    Texture2D* blammoTex;

    Texture2D* sphereNormalsTex;
    CgFxPostRefract normalTexRefractEffect;
    ESPParticleColourEffector particleFader;
    ESPParticleScaleEffector particleSuperGrowth;

    ESPPointEmitter biffShockwave;
    ESPPointEmitter bamShockwave;
    ESPPointEmitter blammoShockwave;

    AnimationMultiLerp<float> biffScaleAnim;
    AnimationMultiLerp<float> bamScaleAnim;
    AnimationMultiLerp<float> blammoScaleAnim;

    AnimationMultiLerp<float> biffAlphaAnim;
    AnimationMultiLerp<float> bamAlphaAnim;
    AnimationMultiLerp<float> blammoAlphaAnim;

    bool isFinishedAnimating;
    
    bool biffSlamEffectDone;
    bool bamSlamEffectDone;
    bool blammoSlamEffectDone;

    void SetupShockwave(float x, float y, ESPPointEmitter& shockwaveEmitter);

    DISALLOW_COPY_AND_ASSIGN(BBBTitleDisplay);
};

inline void BBBTitleDisplay::SetAlpha(float alpha) {
    this->alpha = alpha;
}

inline bool BBBTitleDisplay::IsFinishedAnimating() const {
    return this->isFinishedAnimating && this->biffSlamEffectDone &&
        this->bamSlamEffectDone && this->blammoSlamEffectDone;
}

#endif // __BBBTITLEDISPLAY_H__