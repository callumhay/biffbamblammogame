/**
 * CgFxGaussianBlur.cpp
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

#include "CgFxGaussianBlur.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/GeometryMaker.h"

const char* CgFxGaussianBlur::GAUSSIANBLUR_3x3_TECHNIQUE_NAME = "GaussianBlur3x3";
const char* CgFxGaussianBlur::GAUSSIANBLUR_5x5_TECHNIQUE_NAME = "GaussianBlur5x5";
const char* CgFxGaussianBlur::GAUSSIANBLUR_7x7_TECHNIQUE_NAME = "GaussianBlur7x7";

CgFxGaussianBlur::CgFxGaussianBlur(BlurType blurType, FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_GAUSSIAN_SHADER, sceneFBO),
tempFBO(NULL), sigma(0.8f), isPoisonBlurActive(false) {

	// Set the blur type - this will also set the proper technique for the shader
	this->SetBlurType(blurType);

	// Setup the temporary FBO, used to ping-pong rendering of separable filters
	this->tempFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
	    sceneFBO->GetFBOTexture()->GetHeight(), Texture::Nearest, FBObj::NoAttachment);

	// Establish all the CG parameters in the effect
	this->sceneSamplerParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->blurSizeHorizontalParam = cgGetNamedEffectParameter(this->cgEffect, "BlurSizeHorizontal");
    this->blurSizeVerticalParam   = cgGetNamedEffectParameter(this->cgEffect, "BlurSizeVertical");
	this->sigmaParam              = cgGetNamedEffectParameter(this->cgEffect, "Sigma");
	
    this->poisonBlurAnim.ClearLerp();
    this->poisonBlurAnim.SetInterpolantValue(0.0f);
    this->poisonBlurAnim.SetRepeat(false);
    
	debug_cg_state();
}

CgFxGaussianBlur::~CgFxGaussianBlur() {
	// Clean-up the temporary FBO
	delete this->tempFBO;
	this->tempFBO = NULL;
}

/**
 * Set the blur type - this has the effect of changing the technique used by the
 * shader to the one corresponding to the specified type of blur.
 */
void CgFxGaussianBlur::SetBlurType(BlurType type) {
	switch (type) {
		case Kernel3x3:
			this->currTechnique = this->techniques[GAUSSIANBLUR_3x3_TECHNIQUE_NAME];
			break;
		case Kernel5x5:
			this->currTechnique = this->techniques[GAUSSIANBLUR_5x5_TECHNIQUE_NAME];
			break;
		case Kernel7x7:
			this->currTechnique = this->techniques[GAUSSIANBLUR_7x7_TECHNIQUE_NAME];
			break;
		default:
			this->currTechnique = NULL;
			assert(false);
			break;
	}

	this->blurType = type;
}

/**
 * Gaussian blurs are done as 2 separable filters, the first horizontal
 * and the second vertical (or vice-versa) each filter is a single pass and part of
 * the same technique. Note that the extra parameter is used to add an extra
 * blur effect - kind of like being drunk, if needed.
 */
void CgFxGaussianBlur::Draw(int screenWidth, int screenHeight) {
	
	float revisedWidth  = screenWidth;
	float revisedHeight = screenHeight;

    if (this->isPoisonBlurActive) {
        revisedWidth = this->poisonBlurAnim.GetInterpolantValue();
    }

	// Step 0: Establish uniform parameter(s)
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());
    cgGLSetParameter1f(this->blurSizeHorizontalParam, 1.0f / revisedWidth);
    cgGLSetParameter1f(this->blurSizeVerticalParam,   1.0f / revisedHeight);
    cgGLSetParameter1f(this->sigmaParam, this->sigma);

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
}

/**
 * This will set up the necessary variables for animating the poison effect
 * or disabling it.
 */
void CgFxGaussianBlur::SetPoisonBlurAnimation(bool on) {
	std::vector<double> timeVals;
	std::vector<float> blurVals;

	if (on) {
		// Setup the animation for blurring the width (drunken poison effect)
		timeVals.reserve(5);
		timeVals.push_back(0.0);
		timeVals.push_back(1.0);
		timeVals.push_back(2.0);
		timeVals.push_back(3.0);
		timeVals.push_back(4.0);

		blurVals.reserve(5);
		blurVals.push_back(150.0f);
		blurVals.push_back(70.0f);
		blurVals.push_back(30.0f);
		blurVals.push_back(70.0f);
		blurVals.push_back(150.0f);

		this->poisonBlurAnim.SetLerp(timeVals, blurVals);
		this->poisonBlurAnim.SetRepeat(true);
        this->isPoisonBlurActive = true;
	}
	else {
        if (this->isPoisonBlurActive) {
            timeVals.reserve(2);
            timeVals.push_back(0.0f);
            timeVals.push_back(3.0f);

            blurVals.reserve(2);
            blurVals.push_back(this->poisonBlurAnim.GetInterpolantValue());
            blurVals.push_back(this->sceneFBO->GetFBOTexture()->GetWidth());
            
            this->poisonBlurAnim.SetLerp(timeVals, blurVals);
            this->poisonBlurAnim.SetRepeat(false);
        }
        else {
            // Ignore, the poison blur effect is already inactive
        }
	}
}