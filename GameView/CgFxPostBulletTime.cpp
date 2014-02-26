/**
 * CgFxPostBulletTime.cpp
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

#include "CgFxPostBulletTime.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"


const char* CgFxPostBulletTime::POSTBULLETTIME_TECHNIQUE_NAME = "PostBulletTime";


CgFxPostBulletTime::CgFxPostBulletTime(FBObj* outputFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_POSTBULLETTIME_SHADER, NULL), resultFBO(outputFBO),
sceneSamplerParam(NULL), desaturateFractionParam(NULL), sampleDistanceParam(NULL),
sampleStrengthParam(NULL), desaturateFrac(0.0f), sampleDistance(0.0f), sampleStrength(0.0f) {
    
    this->desaturateFrac.SetRepeat(false);
    this->sampleDistance.SetRepeat(false);
    this->sampleStrength.SetRepeat(false);

	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostBulletTime::POSTBULLETTIME_TECHNIQUE_NAME];

    // Initialize all of the Cg parameters
    this->sceneSamplerParam         = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
    this->desaturateFractionParam   = cgGetNamedEffectParameter(this->cgEffect, "DesaturateFraction");
    this->sampleDistanceParam       = cgGetNamedEffectParameter(this->cgEffect, "SampleDistance");
    this->sampleStrengthParam       = cgGetNamedEffectParameter(this->cgEffect, "SampleStrength");

	debug_cg_state();
}

CgFxPostBulletTime::~CgFxPostBulletTime() {
}

void CgFxPostBulletTime::UpdateBulletTimeState(const BallBoostModel& boostModel) {

    static const float FADE_IN_DESATURATION     = 0.6f;
    static const float FADE_IN_SAMPLE_DISTANCE  = 0.25f;
    static const float FADE_IN_SAMPLE_STRENGTH  = 2.0f;

    static const float MAX_MULTIBALL_SAMPLE_DISTANCE  = 0.35f;
    static const float MAX_MULTIBALL_SAMPLE_STRENGTH  = 3.0f;

    static const float MAX_DESATURATION     = 0.85f;
    static const float MAX_SAMPLE_DISTANCE  = 0.60f;
    static const float MAX_SAMPLE_STRENGTH  = 4.5f;

    switch (boostModel.GetBulletTimeState()) {
        case BallBoostModel::NotInBulletTime:
            this->desaturateFrac.SetInterpolantValue(0.0f);
            this->sampleDistance.SetInterpolantValue(0.0f);
            this->sampleStrength.SetInterpolantValue(0.0f);
            break;

        case BallBoostModel::BulletTimeFadeIn:
            this->desaturateFrac.SetLerp(BallBoostModel::BULLET_TIME_FADE_IN_SECONDS, FADE_IN_DESATURATION);
            this->sampleDistance.SetLerp(BallBoostModel::BULLET_TIME_FADE_IN_SECONDS, FADE_IN_SAMPLE_DISTANCE);
            this->sampleStrength.SetLerp(BallBoostModel::BULLET_TIME_FADE_IN_SECONDS, FADE_IN_SAMPLE_STRENGTH);
            break;

        case BallBoostModel::BulletTimeFadeOut:
            this->desaturateFrac.SetLerp(BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS, 0.0f);
            this->sampleDistance.SetLerp(BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS, 0.0f);
            this->sampleStrength.SetLerp(BallBoostModel::BULLET_TIME_FADE_OUT_SECONDS, 0.0f);
            break;

        case BallBoostModel::BulletTime:
            this->desaturateFrac.SetLerp(BallBoostModel::GetMaxBulletTimeDuration(), MAX_DESATURATION);

            // We diminish the total blur if there's more than one ball and they're far apart from each other
            if (boostModel.GetCurrentNumBalls() > 1 &&
                std::max<float>(boostModel.GetBallZoomBounds().GetWidth(), boostModel.GetBallZoomBounds().GetHeight()) > 2*LevelPiece::PIECE_WIDTH) {
                this->sampleDistance.SetLerp(BallBoostModel::GetMaxBulletTimeDuration(), MAX_MULTIBALL_SAMPLE_DISTANCE);
                this->sampleStrength.SetLerp(BallBoostModel::GetMaxBulletTimeDuration(), MAX_MULTIBALL_SAMPLE_STRENGTH);
            }
            else {
                this->sampleDistance.SetLerp(BallBoostModel::GetMaxBulletTimeDuration(), MAX_SAMPLE_DISTANCE);
                this->sampleStrength.SetLerp(BallBoostModel::GetMaxBulletTimeDuration(), MAX_SAMPLE_STRENGTH);
            }
            break;

        default:
            assert(false);
            break;
    }
}

void CgFxPostBulletTime::UpdateAndDraw(double dT, const BallBoostModel* boostModel,
                                       FBObj*& sceneIn, FBObj*& renderOut) {
    static FBObj* tempSwapFBO = NULL;
    if (boostModel == NULL || boostModel->GetBulletTimeState() == BallBoostModel::NotInBulletTime) {
        return;
    }

    assert(sceneIn != NULL && renderOut != NULL);

    // Draw the effect
    this->sceneFBO  = sceneIn;
    this->resultFBO = renderOut;

    // We need to multiply the dT by the inverse time dialation since we're currently in bullet time
    this->Draw(renderOut->GetFBOTexture()->GetWidth(), 
               renderOut->GetFBOTexture()->GetHeight(), dT * boostModel->GetInverseTimeDialation());

    // Swap the input and output FBOs that were given to this function
	tempSwapFBO = sceneIn;
	sceneIn = renderOut;
	renderOut = tempSwapFBO;
}

void CgFxPostBulletTime::Draw(int screenWidth, int screenHeight, double dT) {
    this->desaturateFrac.Tick(dT);
    this->sampleDistance.Tick(dT);
    this->sampleStrength.Tick(dT);

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->desaturateFractionParam,   this->desaturateFrac.GetInterpolantValue());
	cgGLSetParameter1f(this->sampleDistanceParam,       this->sampleDistance.GetInterpolantValue());
	cgGLSetParameter1f(this->sampleStrengthParam,       this->sampleStrength.GetInterpolantValue());

    cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
}