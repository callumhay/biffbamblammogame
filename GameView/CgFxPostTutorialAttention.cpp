/**
 * CgFxPostTutorialAttention.cpp
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

#include "CgFxPostTutorialAttention.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/GeometryMaker.h"

const char* CgFxPostTutorialAttention::TECHNIQUE_NAME = "PostTutorialAttention";

CgFxPostTutorialAttention::CgFxPostTutorialAttention(FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_POSTTUTORIALATTENTION_SHADER, sceneFBO),
tempFBO(NULL) {

    this->currTechnique = this->techniques[CgFxPostTutorialAttention::TECHNIQUE_NAME];

    // Setup the temporary FBO, used to ping-pong rendering of separable filters
    this->tempFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), 
        Texture::Nearest,
        FBObj::NoAttachment);

	// Establish all the CG parameters in the effect
	this->sceneSamplerParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->blurSizeHorizontalParam = cgGetNamedEffectParameter(this->cgEffect, "BlurSizeHorizontal");
    this->blurSizeVerticalParam   = cgGetNamedEffectParameter(this->cgEffect, "BlurSizeVertical");
	this->sigmaParam              = cgGetNamedEffectParameter(this->cgEffect, "Sigma");
    this->greyscaleFactorParam    = cgGetNamedEffectParameter(this->cgEffect, "GreyscaleFactor");
    
    this->SetAlpha(0.0f);

	debug_cg_state();
}

CgFxPostTutorialAttention::~CgFxPostTutorialAttention() {
    delete this->tempFBO;
    this->tempFBO = NULL;
}

void CgFxPostTutorialAttention::SetAlpha(float alpha) {
    this->fadeAnim.ClearLerp();
    this->fadeAnim.SetInterpolantValue(alpha);
    this->fadeAnim.SetRepeat(false);
}

void CgFxPostTutorialAttention::CrossFadeIn(double fadeInTime) {
    // Check to see if we're already fading in or have faded in...
    if (this->fadeAnim.GetTargetValue() == 1.0f && this->fadeAnim.GetInterpolantValue() == 1.0f) {
        return;
    }

    double crossFadeInTime = NumberFuncs::LerpOverFloat<double>(0.0f, 1.0f, fadeInTime, 0.0, this->fadeAnim.GetInterpolantValue());
    this->fadeAnim.SetLerp(crossFadeInTime, 1.0f);
    this->fadeAnim.SetRepeat(false);
}

void CgFxPostTutorialAttention::CrossFadeOut(double fadeOutTime) {
    // Check to see if we're already fading out or have faded out...
    if (this->fadeAnim.GetTargetValue() == 0.0f && this->fadeAnim.GetInterpolantValue() == 0.0f) {
        return;
    }

    double crossFadeOutTime = NumberFuncs::LerpOverFloat<double>(1.0f, 0.0f, fadeOutTime, 0.0, this->fadeAnim.GetInterpolantValue());
    this->fadeAnim.SetLerp(crossFadeOutTime, 0.0f);
    this->fadeAnim.SetRepeat(false);
}

void CgFxPostTutorialAttention::Draw(int screenWidth, int screenHeight, double dT) {
	
    // Get the fade amount, a value in [0, 1] representing no effect to full effect
    float fadeAmt = this->fadeAnim.GetInterpolantValue();
    this->fadeAnim.Tick(dT);

    // Don't apply the effect if it's not active
    if (fadeAmt <= 0.0f) {
        return;
    }

	// Step 0: Establish uniform parameter(s)
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());
    cgGLSetParameter1f(this->blurSizeHorizontalParam, 1.0f / static_cast<float>(screenWidth));
    cgGLSetParameter1f(this->blurSizeVerticalParam,   1.0f / static_cast<float>(screenHeight));
    cgGLSetParameter1f(this->sigmaParam, NumberFuncs::LerpOverFloat(0.0f, 1.0f, 0.001f, 2.0f, fadeAmt));
    cgGLSetParameter1f(this->greyscaleFactorParam, NumberFuncs::LerpOverFloat(0.0f, 1.0f, 0.0f, 0.55f, fadeAmt));

	// Step 1: Bind the temporary FBO and draw a full-screen quad with the first pass of the effect.
	this->tempFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);

	// Step 2: We have the first blur stored into the temporary FBO, now feed that
	// into the next pass and render it into the scene FBO.
	cgGLSetTextureParameter(this->sceneSamplerParam, this->tempFBO->GetFBOTexture()->GetTextureID());
	this->sceneFBO->BindFBObj();
	currPass = cgGetNextPass(currPass);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);	

	this->sceneFBO->UnbindFBObj();

    debug_cg_state();
}
