/**
 * PaddleStatusEffectRenderer.h
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

#ifndef __PADDLESTATUSEFFECTRENDERER_H__
#define __PADDLESTATUSEFFECTRENDERER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../ESPEngine/ESP.h"
#include "../GameSound/SoundCommon.h"

#include "CgFxPostRefract.h"
#include "CgFxFireBallEffect.h"
#include "CgFxVolumetricEffect.h"

class GameESPAssets;
class GameSound;
class PlayerPaddle;
class Texture2D;

class PaddleStatusEffectRenderer {
public:
    PaddleStatusEffectRenderer(GameESPAssets* espAssets, GameSound* sound);
    ~PaddleStatusEffectRenderer();

    CgFxEffectBase* GetPaddleStatusMaterial() const;

    void ToggleFrozen(const PlayerPaddle& paddle, bool isFrozen);
    void ToggleOnFire(const PlayerPaddle& paddle, bool isOnFire);
    
    void CancelOnFireWithFrozen();
    void CancelFrozenWithOnFire();

    void Reinitialize();

    void Draw(double dT, const Camera& camera, const PlayerPaddle& paddle, const Texture2D* sceneTexture);

private:
    GameESPAssets* espAssets;
    GameSound* sound;

    CgFxEffectBase* currStatusMaterial;

    // Frozen-status-related variables -----------------
    static const double TIME_BETWEEN_FROZEN_SHAKES_IN_SECS;

    Texture2D* rectPrismTexture;
    Texture2D* frostTexture;

    CgFxPostRefract iceBlockEffect;
    AnimationMultiLerp<float> paddleIceShakeAnim;
    double frozenShakeTimeCounter;

    // On-fire-status-related variables ----------------
    static const double TIME_BETWEEN_ON_FIRE_NORMAL_FLASHES_IN_SECS;
    CgFxFireBallEffect onFireEffect;
    AnimationMultiLerp<Colour> fireFlashAnim;
    double onFireNormalFlashTimeCounter;

    ESPLineEmitter* paddleOnFireEmitter;
    CgFxVolumetricEffect paddleFireEmitterEffect;
    std::vector<Texture2D*> cloudTextures;
    ESPParticleColourEffector fireColourFader;
    ESPParticleScaleEffector particleMediumGrowth;
    ESPParticleAccelEffector fireAccel;

    SoundID paddleIceShakeSoundID;

    DISALLOW_COPY_AND_ASSIGN(PaddleStatusEffectRenderer);
};

inline CgFxEffectBase* PaddleStatusEffectRenderer::GetPaddleStatusMaterial() const {
    return this->currStatusMaterial;
}

#endif // __PADDLESTATUSEFFECTRENDERER_H__