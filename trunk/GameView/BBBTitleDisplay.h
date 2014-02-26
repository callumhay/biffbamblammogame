/**
 * BBBTitleDisplay.h
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
    return this->isFinishedAnimating;
}

#endif // __BBBTITLEDISPLAY_H__