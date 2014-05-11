/**
 * CgFxBloom.cpp
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

#include "CgFxBloom.h"
#include "CgFxGaussianBlur.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"

const char* CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME           = "BloomFilter";
const char* CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME	     = "CompositeBloom";

const float CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD	= 0.6f;
const float CgFxBloom::DEFAULT_SCENE_INTENSITY		= 1.0f;
const float CgFxBloom::DEFAULT_GLOW_INTENSITY		= 0.33333f;
const float CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY	= 0.05f;

CgFxBloom::CgFxBloom(FBObj* sceneFBO, CgFxGaussianBlur* blurEffect) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_BLOOM_SHADER, sceneFBO), blurEffect(blurEffect),
sceneSamplerParam(NULL),highlightThresholdParam(NULL), sceneWidthParam(NULL), 
sceneIntensity(CgFxBloom::DEFAULT_SCENE_INTENSITY), glowIntensity(CgFxBloom::DEFAULT_GLOW_INTENSITY), 
highlightIntensity(CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY), highlightThreshold(CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD),
sceneHeightParam(NULL), sceneIntensityParam(NULL), glowIntensityParam(NULL), highlightIntensityParam(NULL),
bloomFilterFBO(NULL) {

    assert(blurEffect != NULL);

	// Setup the temporary FBOs, used to ping-pong rendering of separable filters and store intermediate renderings
	this->bloomFilterFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), Texture::Bilinear, FBObj::NoAttachment);

	// Setup any CG parameters
	this->sceneSamplerParam         = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
    this->brightBlurSamplerParam    = cgGetNamedEffectParameter(this->cgEffect, "BrightBlurSampler");

    this->brightBlurSamplerMip3Param = cgGetNamedEffectParameter(this->cgEffect, "BrightBlurSamplerMip3");
    this->brightBlurSamplerMip4Param = cgGetNamedEffectParameter(this->cgEffect, "BrightBlurSamplerMip4");

	this->sceneWidthParam  = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	this->highlightThresholdParam = cgGetNamedEffectParameter(this->cgEffect, "HighlightThreshold");
	this->sceneIntensityParam     = cgGetNamedEffectParameter(this->cgEffect, "SceneIntensity");
	this->glowIntensityParam      = cgGetNamedEffectParameter(this->cgEffect, "GlowIntensity");
	this->highlightIntensityParam = cgGetNamedEffectParameter(this->cgEffect, "HighlightIntensity");

	debug_cg_state();
	debug_opengl_state();
}

CgFxBloom::~CgFxBloom() {
	// Clean-up the temporary FBO
	delete this->bloomFilterFBO;
	this->bloomFilterFBO = NULL;
}

void CgFxBloom::Draw(int screenWidth, int screenHeight, double dT) {
	UNUSED_PARAMETER(dT);

	// Step 0: Setup necessary cg parameters
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	cgGLSetParameter1f(this->sceneWidthParam,  screenWidth);
	cgGLSetParameter1f(this->sceneHeightParam, screenHeight);

	cgGLSetParameter1f(this->highlightThresholdParam, this->highlightThreshold);
	cgGLSetParameter1f(this->sceneIntensityParam, this->sceneIntensity);
	cgGLSetParameter1f(this->glowIntensityParam, this->glowIntensity);
	cgGLSetParameter1f(this->highlightIntensityParam, this->highlightIntensity);

	// Step 1: Draw a full screen quad with the bloom filter applied to it
	// render it into a temporary FBO - this will grab all the very bright parts
	// of the screen
	this->bloomFilterFBO->BindFBObj();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CGpass currPass = cgGetFirstPass(this->techniques[CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);

    // Step 2: Using the Gaussian blur effect, blur only the most luminous parts of the scene acquired in step 1
    this->blurEffect->SetInputFBO(this->bloomFilterFBO);
    this->blurEffect->SetBlurType(CgFxGaussianBlur::Kernel5x5);
    this->blurEffect->SetSigma(10.0f);
    this->blurEffect->Draw(screenWidth, screenHeight);

    // Step 3: Final bloom composition that will combine the original scene with the blurred luminance buffer
    cgGLSetTextureParameter(this->brightBlurSamplerParam, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());
    cgGLSetTextureParameter(this->brightBlurSamplerMip3Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());
    cgGLSetTextureParameter(this->brightBlurSamplerMip4Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());

	this->sceneFBO->BindFBObj();

	currPass = cgGetFirstPass(this->techniques[CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight, -1.0f);
	cgResetPassState(currPass);

	FBObj::UnbindFBObj();
}
