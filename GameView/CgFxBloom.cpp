/**
 * CgFxBloom.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxBloom.h"
#include "CgFxGaussianBlur.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"

const char* CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME           = "BloomFilter";
const char* CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME	     = "CompositeBloom";

const float CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD	= 0.58f;
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

    // Step 2: Using the gaussian blur effect, blur only the most luminous parts of the scene acquired in step 1
    this->blurEffect->SetInputFBO(this->bloomFilterFBO);
    this->blurEffect->SetBlurType(CgFxGaussianBlur::Kernel5x5);
    this->blurEffect->SetSigma(10.0f);
    this->blurEffect->Draw(screenWidth, screenHeight, dT);

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
