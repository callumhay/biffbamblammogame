/**
 * PaddleStatusEffectRenderer.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLESTATUSEFFECTRENDERER_H__
#define __PADDLESTATUSEFFECTRENDERER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../ESPEngine/ESP.h"
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

    DISALLOW_COPY_AND_ASSIGN(PaddleStatusEffectRenderer);
};

inline CgFxEffectBase* PaddleStatusEffectRenderer::GetPaddleStatusMaterial() const {
    return this->currStatusMaterial;
}

#endif // __PADDLESTATUSEFFECTRENDERER_H__