/**
 * BBBTitleDisplay.cpp
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

#include "BBBTitleDisplay.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../GameSound/GameSound.h"

#include "../ResourceManager.h"

BBBTitleDisplay::BBBTitleDisplay(float scaleFactor, double fadeInTime, GameSound* sound) : 
scaleFactor(scaleFactor), alpha(1.0f), sound(sound), biffTex(NULL), bamTex(NULL), blammoTex(NULL),
sphereNormalsTex(NULL), isFinishedAnimating(false), particleFader(1, 0), particleSuperGrowth(1.0f, 20.0f),
biffSlamEffectDone(false), bamSlamEffectDone(false), blammoSlamEffectDone(false) {

    this->biffTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BIFF_LOGO, Texture::Trilinear));
    assert(this->biffTex != NULL);
    this->bamTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BAM_LOGO, Texture::Trilinear));
    assert(this->bamTex != NULL);
    this->blammoTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BLAMMO_LOGO, Texture::Trilinear));
    assert(this->blammoTex != NULL);

    static const float STARTING_SCALE_FACTOR = 10.0f;

    static const double FIRST_DELAY  = 0.42;
    static const double SECOND_DELAY = 0.39;

    // Setup the animations
    std::vector<double> scaleTimeVals;
    scaleTimeVals.reserve(3);
    scaleTimeVals.push_back(fadeInTime);
    scaleTimeVals.push_back(fadeInTime + 0.22);
    scaleTimeVals.push_back(fadeInTime + 0.225);

    std::vector<float> scaleVals;
    scaleVals.reserve(scaleTimeVals.size());
    scaleVals.push_back(STARTING_SCALE_FACTOR);
    scaleVals.push_back(0.75f);
    scaleVals.push_back(1.0f);

    std::vector<double> alphaTimeVals;
    alphaTimeVals.reserve(2);
    alphaTimeVals.push_back(fadeInTime);
    alphaTimeVals.push_back(fadeInTime + 0.15);

    std::vector<float> alphaVals;
    alphaVals.reserve(alphaTimeVals.size());
    alphaVals.push_back(0.0f);
    alphaVals.push_back(1.0f);

    this->biffScaleAnim.SetLerp(scaleTimeVals, scaleVals);
    this->biffScaleAnim.SetRepeat(false);
    this->biffScaleAnim.SetInterpolantValue(STARTING_SCALE_FACTOR);
    
    this->biffAlphaAnim.SetLerp(alphaTimeVals, alphaVals);
    this->biffAlphaAnim.SetRepeat(false);
    this->biffAlphaAnim.SetInterpolantValue(0.0f);

    std::transform(scaleTimeVals.begin(), scaleTimeVals.end(), scaleTimeVals.begin(), std::bind2nd(std::plus<double>(), FIRST_DELAY));
    std::transform(alphaTimeVals.begin(), alphaTimeVals.end(), alphaTimeVals.begin(), std::bind2nd(std::plus<double>(), FIRST_DELAY));
    this->bamScaleAnim.SetLerp(scaleTimeVals, scaleVals);
    this->bamScaleAnim.SetInterpolantValue(STARTING_SCALE_FACTOR);

    this->bamAlphaAnim.SetLerp(alphaTimeVals, alphaVals);
    this->bamAlphaAnim.SetRepeat(false);
    this->bamAlphaAnim.SetInterpolantValue(0.0f);

    std::transform(scaleTimeVals.begin(), scaleTimeVals.end(), scaleTimeVals.begin(), std::bind2nd(std::plus<double>(), SECOND_DELAY));
    std::transform(alphaTimeVals.begin(), alphaTimeVals.end(), alphaTimeVals.begin(), std::bind2nd(std::plus<double>(), SECOND_DELAY));
    this->blammoScaleAnim.SetLerp(scaleTimeVals, scaleVals);
    this->blammoScaleAnim.SetInterpolantValue(STARTING_SCALE_FACTOR);

    this->blammoAlphaAnim.SetLerp(alphaTimeVals, alphaVals);
    this->blammoAlphaAnim.SetRepeat(false);
    this->blammoAlphaAnim.SetInterpolantValue(0.0f);


    this->sphereNormalsTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_SPHERE_NORMALS, Texture::Trilinear));
    assert(this->sphereNormalsTex != NULL);

    this->normalTexRefractEffect.SetTechnique(CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME);
    this->normalTexRefractEffect.SetWarpAmountParam(27.0f);
    this->normalTexRefractEffect.SetIndexOfRefraction(1.2f);
    this->normalTexRefractEffect.SetNormalTexture(this->sphereNormalsTex);

    this->SetupShockwave(-324*scaleFactor, 157*scaleFactor, this->biffShockwave);
    this->SetupShockwave(-130*scaleFactor, -90*scaleFactor, this->bamShockwave);
    this->SetupShockwave( 241*scaleFactor, -40*scaleFactor, this->blammoShockwave);
}

BBBTitleDisplay::~BBBTitleDisplay() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->biffTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bamTex);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->blammoTex);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->sphereNormalsTex);
    assert(success);

    UNUSED_VARIABLE(success);
}


void BBBTitleDisplay::Draw(double dT, Camera& camera, const Texture2D* fboTex) {
    const float VERT_DIST_FROM_EDGE = this->scaleFactor * -25;

    // Setup the base size values
    float xSize = 1.05f * this->scaleFactor * 1024.0f;
    float ySize = 1.05f * this->scaleFactor * 512.0f;

    // Tick all of the animations
    bool biffDoneAnimating = this->biffScaleAnim.Tick(dT);
    biffDoneAnimating &= this->biffAlphaAnim.Tick(dT);

    bool bamDoneAnimating = this->bamScaleAnim.Tick(dT);
    bamDoneAnimating &= this->bamAlphaAnim.Tick(dT);

    bool blammoDoneAnimating = this->blammoScaleAnim.Tick(dT);
    blammoDoneAnimating &= this->blammoAlphaAnim.Tick(dT);

    this->isFinishedAnimating = blammoDoneAnimating && bamDoneAnimating && biffDoneAnimating; 

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Camera::PushWindowCoords();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Always translate to the same center, the elements of the title will scale about this center
    glTranslatef((Camera::GetWindowWidth() - xSize) / 2.0f + xSize / 2.0f, 
        Camera::GetWindowHeight() - ySize / 2.0f - VERT_DIST_FROM_EDGE, 0);

    static const double CAMERA_SHAKE_DURATION  = 0.6;
    static const float CAMERA_BASE_SHAKE_MAG   = 0.2f;
    static const float CAMERA_BASE_SHAKE_SPEED = 85.0f;

    // Draw the after effects when necessary
    if (biffDoneAnimating) {
        //this->biffSlamEffectsDone = true;
        this->normalTexRefractEffect.SetFBOTexture(fboTex);
        this->biffShockwave.Tick(dT);
        this->biffShockwave.Draw(camera);

        if (!this->biffSlamEffectDone) {
            this->sound->PlaySound(GameSound::MainMenuTitleBiffSlamEvent, false, false);
            camera.ApplyCameraShake(CAMERA_SHAKE_DURATION, Vector3D(CAMERA_BASE_SHAKE_MAG, CAMERA_BASE_SHAKE_MAG, 0), CAMERA_BASE_SHAKE_SPEED);
            this->biffSlamEffectDone = true;
        }

        if (bamDoneAnimating) {
            this->bamShockwave.Tick(dT);
            this->bamShockwave.Draw(camera);

            if (!this->bamSlamEffectDone) {
                this->sound->PlaySound(GameSound::MainMenuTitleBamSlamEvent, false, false);
                camera.ApplyCameraShake(CAMERA_SHAKE_DURATION, Vector3D(CAMERA_BASE_SHAKE_MAG, CAMERA_BASE_SHAKE_MAG, 0), CAMERA_BASE_SHAKE_SPEED);
                this->bamSlamEffectDone = true;
            }

            if (blammoDoneAnimating) {
                this->blammoShockwave.Tick(dT);
                this->blammoShockwave.Draw(camera);

                if (!this->blammoSlamEffectDone) {
                    this->sound->PlaySound(GameSound::MainMenuTitleBlammoSlamEvent, false, false);
                    camera.ApplyCameraShake(CAMERA_SHAKE_DURATION, Vector3D(CAMERA_BASE_SHAKE_MAG, CAMERA_BASE_SHAKE_MAG, 0), CAMERA_BASE_SHAKE_SPEED);
                    this->blammoSlamEffectDone = true;
                }
            }
        }
    }

    // Draw "Biff!"
    float biffScale = this->biffScaleAnim.GetInterpolantValue();
    glPushMatrix();
    glScalef(xSize*biffScale, ySize*biffScale, 1.0f);
    this->biffTex->BindTexture();
    glColor4f(1, 1, 1, this->alpha * this->biffAlphaAnim.GetInterpolantValue());
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    // Draw "Bam!!"
    float bamScale = this->bamScaleAnim.GetInterpolantValue();
    glPushMatrix();
    glScalef(xSize*bamScale, ySize*bamScale, 1.0f);
    glColor4f(1, 1, 1, this->alpha * this->bamAlphaAnim.GetInterpolantValue());
    this->bamTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    // Draw "Blammo!?!"
    float blammoScale = this->blammoScaleAnim.GetInterpolantValue();
    glPushMatrix();
    glScalef(xSize*blammoScale, ySize*blammoScale, 1.0f);
    this->blammoTex->BindTexture();
    glColor4f(1, 1, 1, this->alpha * this->blammoAlphaAnim.GetInterpolantValue());
    GeometryMaker::GetInstance()->DrawQuad();
    glPopMatrix();

    glPopMatrix();
    Camera::PopWindowCoords();

    glPopAttrib();
}

void BBBTitleDisplay::SetupShockwave(float x, float y, ESPPointEmitter& shockwaveEmitter) {
    shockwaveEmitter.SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
    shockwaveEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
    shockwaveEmitter.SetParticleLife(ESPInterval(0.33f));
    shockwaveEmitter.SetParticleSize(ESPInterval(scaleFactor * 128));
    shockwaveEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    shockwaveEmitter.SetParticleAlignment(ESP::NoAlignment);
    shockwaveEmitter.SetEmitPosition(Point3D(x, y, 0));
    shockwaveEmitter.SetEmitDirection(Vector3D(0,1,0));
    shockwaveEmitter.SetToggleEmitOnPlane(true);
    shockwaveEmitter.SetParticleColour(ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f), ESPInterval(1.0f));
    shockwaveEmitter.AddEffector(&this->particleFader);
    shockwaveEmitter.AddEffector(&this->particleSuperGrowth);
    shockwaveEmitter.SetParticles(1, &this->normalTexRefractEffect);
}