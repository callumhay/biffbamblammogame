/**
 * PaddleStatusEffectRenderer.cpp
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

#include "PaddleStatusEffectRenderer.h"
#include "GameViewConstants.h"

#include "../GameSound/GameSound.h"

#include "../GameControl/GameControllerManager.h"

const double PaddleStatusEffectRenderer::TIME_BETWEEN_FROZEN_SHAKES_IN_SECS          = 0.125;
const double PaddleStatusEffectRenderer::TIME_BETWEEN_ON_FIRE_NORMAL_FLASHES_IN_SECS = 0.03;

PaddleStatusEffectRenderer::PaddleStatusEffectRenderer(GameESPAssets* espAssets, GameSound* sound) :
espAssets(espAssets), sound(sound), currStatusMaterial(NULL), frozenShakeTimeCounter(0.0),
rectPrismTexture(NULL), frostTexture(NULL), paddleOnFireEmitter(NULL),
fireColourFader(ColourRGBA(1.0f, 1.0f, 0.0f, 1.0f), ColourRGBA(0.4f, 0.15f, 0.0f, 0.2f)),
particleMediumGrowth(1.0f, 1.6f), fireAccel(Vector3D(1,1,1)) {
    
    assert(espAssets != NULL);
    assert(sound != NULL);

    assert(this->rectPrismTexture == NULL);
    this->rectPrismTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_RECT_PRISM_NORMALS, Texture::Trilinear));
    assert(this->rectPrismTexture != NULL);

    assert(this->frostTexture == NULL);
    this->frostTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_FROST, Texture::Trilinear));
    assert(this->frostTexture != NULL);

    this->iceBlockEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_WITH_OVERLAY_TECHNIQUE_NAME);
    this->iceBlockEffect.SetWarpAmountParam(35.0f);
    this->iceBlockEffect.SetIndexOfRefraction(1.31f);
    this->iceBlockEffect.SetNormalTexture(this->rectPrismTexture);
    this->iceBlockEffect.SetOverlayTexture(this->frostTexture);

    this->paddleIceShakeAnim.ClearLerp();
    this->paddleIceShakeAnim.SetInterpolantValue(0.0f);

    this->onFireEffect.SetTechnique(CgFxFireBallEffect::PADDLE_ON_FIRE_TECHNIQUE_NAME);
    this->onFireEffect.SetDarkFireColour(Colour(0.5f, 0.1f, 0.05f));
    this->onFireEffect.SetBrightFireColour(Colour(1.0f, 0.9f, 0.15f));
    this->onFireEffect.SetScale(0.2f);
    this->onFireEffect.SetFrequency(4.0f);

    {   
        const double FLASH_TIME = 0.05;
        const double QUICK_INTERVAL_TIME = 0.001;

        std::vector<double> timeVals;
        timeVals.reserve(5);
        timeVals.push_back(0.0);
        timeVals.push_back(FLASH_TIME);
        timeVals.push_back(timeVals.back() + QUICK_INTERVAL_TIME);
        timeVals.push_back(timeVals.back() + FLASH_TIME);
        timeVals.push_back(timeVals.back() + QUICK_INTERVAL_TIME);

        const Colour BRIGHT_COLOUR(1.0f, 0.9f, 0.15f);
        const Colour DARK_COLOUR(1.0f, 0.4f, 0.0f);

        std::vector<Colour> colourVals;
        colourVals.reserve(timeVals.size());
        colourVals.push_back(BRIGHT_COLOUR);
        colourVals.push_back(BRIGHT_COLOUR);
        colourVals.push_back(DARK_COLOUR);
        colourVals.push_back(DARK_COLOUR);
        colourVals.push_back(BRIGHT_COLOUR);

        this->fireFlashAnim.SetLerp(timeVals, colourVals);
        this->fireFlashAnim.SetRepeat(true);
    }

    // Initialize textures...
    assert(this->cloudTextures.empty());
    this->cloudTextures.reserve(3);
    Texture2D* temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD1, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);
    temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD2, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);
    temp = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLOUD3, Texture::Trilinear));
    assert(temp != NULL);
    this->cloudTextures.push_back(temp);

    this->paddleFireEmitterEffect.SetTechnique(CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME);
    this->paddleFireEmitterEffect.SetColour(Colour(1.0f, 1.0f, 1.0f));
    this->paddleFireEmitterEffect.SetScale(0.8f);
    this->paddleFireEmitterEffect.SetFrequency(1.2f);
    this->paddleFireEmitterEffect.SetFlowDirection(Vector3D(0, 0, 1));
    this->paddleFireEmitterEffect.SetTexture(this->cloudTextures[0]);

    assert(this->paddleOnFireEmitter == NULL);
    static const float MAX_FIRE_PARTICLE_LIFE = 0.75f;
    this->paddleOnFireEmitter = new ESPLineEmitter();
    this->paddleOnFireEmitter->SetEmitLine(Collision::LineSeg3D(Point3D(-PlayerPaddle::PADDLE_HALF_WIDTH,0,0), Point3D(PlayerPaddle::PADDLE_HALF_WIDTH,0,0)));
    this->paddleOnFireEmitter->SetSpawnDelta(ESPInterval(0.005f, 0.02f));
    this->paddleOnFireEmitter->SetInitialSpd(ESPInterval(1.0f, 4.0f));
    this->paddleOnFireEmitter->SetParticleLife(ESPInterval(0.66f*MAX_FIRE_PARTICLE_LIFE, MAX_FIRE_PARTICLE_LIFE));
    this->paddleOnFireEmitter->SetNumParticleLives(std::max<int>(1, static_cast<int>(0.9*PlayerPaddle::PADDLE_ON_FIRE_TIME_IN_SECS / MAX_FIRE_PARTICLE_LIFE)));
    this->paddleOnFireEmitter->SetParticleSize(ESPInterval(0.25f*PlayerPaddle::PADDLE_HALF_WIDTH, 0.75f*PlayerPaddle::PADDLE_HALF_WIDTH));
    this->paddleOnFireEmitter->SetEmitAngle(10);
    this->paddleOnFireEmitter->SetEmitDirection(Vector3D(0,1,0));
    this->paddleOnFireEmitter->SetParticleRotation(ESPInterval(-180.0f, 180.0f));
    this->paddleOnFireEmitter->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
    this->paddleOnFireEmitter->SetParticleAlignment(ESP::ScreenAligned);
    this->paddleOnFireEmitter->AddEffector(&this->fireColourFader);
    this->paddleOnFireEmitter->AddEffector(&this->particleMediumGrowth);
    this->paddleOnFireEmitter->AddEffector(&this->fireAccel);
    this->paddleOnFireEmitter->SetRandomTextureEffectParticles(40, &this->paddleFireEmitterEffect, this->cloudTextures);

}

PaddleStatusEffectRenderer::~PaddleStatusEffectRenderer() {
    bool removed = false;
    
    removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->rectPrismTexture);
    assert(removed);
    removed = ResourceManager::GetInstance()->ReleaseTextureResource(this->frostTexture);
    assert(removed);

    for (std::vector<Texture2D*>::iterator iter = this->cloudTextures.begin();
        iter != this->cloudTextures.end(); ++iter) {

        removed = ResourceManager::GetInstance()->ReleaseTextureResource(*iter);
        assert(removed);	
    }
    this->cloudTextures.clear();

    UNUSED_VARIABLE(removed);

    delete this->paddleOnFireEmitter;
    this->paddleOnFireEmitter = NULL;
}

void PaddleStatusEffectRenderer::ToggleFrozen(const PlayerPaddle& paddle, bool isFrozen) {

    if (isFrozen) {
        static const int NUM_SHAKE_VALS = 10;
        std::vector<double> timeVals;
        timeVals.reserve(10);
        timeVals.push_back(0.0);
        for (int i = 1; i < NUM_SHAKE_VALS; i++) {
            timeVals.push_back(timeVals.back() + 0.02 + 0.03*Randomizer::GetInstance()->RandomNumZeroToOne());
        }
        std::vector<float> shakeVals;
        shakeVals.reserve(timeVals.size());
        float neg = 1.0f;
        for (int i = 0; i < NUM_SHAKE_VALS; i++) {
            shakeVals.push_back(neg *(0.075f + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.01f * paddle.GetHalfWidthTotal()));
            neg = -neg;
        }

        this->paddleIceShakeAnim.SetLerp(timeVals, shakeVals);
        this->paddleIceShakeAnim.SetRepeat(true);

        this->frozenShakeTimeCounter = 0.0;
            
        // The paddle is encased in ice!
        this->sound->PlaySoundAtPosition(GameSound::PaddleFrozenEvent, false, paddle.GetPosition3D(), true, true, true);
    }
    else {
        // The paddle has escaped its icy prison!
        this->sound->PlaySoundAtPosition(GameSound::IceShatterEvent, false, paddle.GetPosition3D(), true, true, true);
    }
}

void PaddleStatusEffectRenderer::ToggleOnFire(const PlayerPaddle& paddle, bool isOnFire) {
    this->onFireNormalFlashTimeCounter = 0.0;
    
    if (isOnFire) {
        this->onFireEffect.SetAlphaMultiplier(1.0f);
        this->currStatusMaterial = &this->onFireEffect;

        this->paddleOnFireEmitter->Reset();

        // Paddle is on fire for a brief amount of time!
        this->sound->PlaySoundAtPosition(GameSound::PaddleOnFireEvent, false, paddle.GetPosition3D(), true, true, true);
    }
    else {
        this->onFireEffect.SetAlphaMultiplier(0.0f);
        this->currStatusMaterial = NULL;
    }
}

void PaddleStatusEffectRenderer::CancelOnFireWithFrozen() {
    this->onFireEffect.SetAlphaMultiplier(0.0f);
    this->currStatusMaterial = NULL;
    this->onFireNormalFlashTimeCounter = 0.0;
}

void PaddleStatusEffectRenderer::CancelFrozenWithOnFire() {
    this->frozenShakeTimeCounter = 0.0;
}

void PaddleStatusEffectRenderer::Reinitialize() {
    this->frozenShakeTimeCounter = 0.0;
    this->onFireNormalFlashTimeCounter = 0.0;
    
    this->paddleIceShakeAnim.ClearLerp();
    this->paddleIceShakeAnim.SetInterpolantValue(0.0f);

    this->currStatusMaterial = NULL;
}

void PaddleStatusEffectRenderer::Draw(double dT, const Camera& camera, const PlayerPaddle& paddle, const Texture2D* sceneTexture) {

    if (paddle.HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {

        this->iceBlockEffect.SetFBOTexture(sceneTexture);
        glPushMatrix();

        if (paddle.GetTimeUntilUnfrozen() <= PlayerPaddle::PADDLE_FROZEN_TIME_IN_SECS/2.0) {
            float shakeAmt = this->paddleIceShakeAnim.GetInterpolantValue();
            glTranslatef(shakeAmt, shakeAmt, 0.0f);
            this->paddleIceShakeAnim.Tick(dT);

            // Shake the controller a little bit
            this->frozenShakeTimeCounter += dT;
            if (paddle.GetTimeUntilUnfrozen() >= TIME_BETWEEN_FROZEN_SHAKES_IN_SECS &&
                this->frozenShakeTimeCounter >= TIME_BETWEEN_FROZEN_SHAKES_IN_SECS) {

                static bool alternator = true;

                if (alternator) {
                    GameControllerManager::GetInstance()->VibrateControllers(
                        TIME_BETWEEN_FROZEN_SHAKES_IN_SECS, BBBGameController::VerySoftVibration, BBBGameController::NoVibration);
                }
                else {
                    GameControllerManager::GetInstance()->VibrateControllers(
                        TIME_BETWEEN_FROZEN_SHAKES_IN_SECS, BBBGameController::NoVibration, BBBGameController::VerySoftVibration);
                }

                this->frozenShakeTimeCounter = 0.0;
                alternator = !alternator;
            }
        }

        glRotatef(paddle.GetZRotation(), 0, 0, 1);
        glScalef(2.0f * paddle.GetHalfWidthTotal() * 1.2f, 2.0f * paddle.GetHalfHeight() * 1.2f, 2.0f * paddle.GetHalfDepthTotal() * 3.0f);
        this->iceBlockEffect.Draw(camera, GeometryMaker::GetInstance()->GetBasicPaddleDL());
        glPopMatrix();
    }

    // Check to see if the paddle is currently on fire
    if (paddle.HasSpecialStatus(PlayerPaddle::OnFireStatus)) {

        Vector2D accelVec = Vector2D::Rotate(static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne() * 45), Vector2D(0,1));
        accelVec *= Randomizer::GetInstance()->RandomNumZeroToOne()*20.0f;
        this->fireAccel.SetAcceleration(Vector3D(accelVec, 0.0f));

        // Flash the fire colours in the fire status material
        this->fireFlashAnim.Tick(dT);
        this->onFireEffect.SetBrightFireColour(this->fireFlashAnim.GetInterpolantValue());
        //this->onFireEffect.SetAlphaMultiplier(paddle.GetTimeUntilNotOnFire());

        static const double TIME_TO_START_FADE_OUT = 0.75;
        if (paddle.GetTimeUntilNotOnFire() < TIME_TO_START_FADE_OUT) {

            float maxAlpha = static_cast<float>(NumberFuncs::LerpOverTime(TIME_TO_START_FADE_OUT, 0.0f, 1.0f, 0.0f, paddle.GetTimeUntilNotOnFire()));
            this->paddleOnFireEmitter->SetParticleAlpha(ESPInterval(maxAlpha));

            // Make the paddle flash between being on fire and its normal form when the fire is almost done
            this->onFireNormalFlashTimeCounter += dT;
            if (this->onFireNormalFlashTimeCounter >= TIME_BETWEEN_ON_FIRE_NORMAL_FLASHES_IN_SECS) {

                // Alternate materials for the paddle to simulate the flashing
                if (this->currStatusMaterial != &this->onFireEffect) {
                    this->currStatusMaterial = &this->onFireEffect;
                } 
                else {
                    this->currStatusMaterial = NULL;
                }
                this->onFireNormalFlashTimeCounter = 0.0;
            }
        }

        this->paddleOnFireEmitter->SetEmitLine(Collision::LineSeg3D(
            Point3D(-paddle.GetHalfWidthTotal(),0,0), 
            Point3D(paddle.GetHalfWidthTotal(),0,0)));
        this->paddleOnFireEmitter->Tick(dT);
        this->paddleOnFireEmitter->Draw(camera);
    }
}