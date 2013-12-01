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
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"

const char* CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME           = "BloomFilter";
const char* CgFxBloom::BRIGHT_DOWNSAMPLE_LVL2_TECHNIQUE_NAME = "BrightDownsampleBlur2";
const char* CgFxBloom::BRIGHT_DOWNSAMPLE_LVL3_TECHNIQUE_NAME = "BrightDownsampleBlur3";
const char* CgFxBloom::BRIGHT_DOWNSAMPLE_LVL4_TECHNIQUE_NAME = "BrightDownsampleBlur4";
const char* CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME	     = "CompositeBloom";

const float CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD	= 0.65f;
const float CgFxBloom::DEFAULT_SCENE_INTENSITY		= 1.0f;
const float CgFxBloom::DEFAULT_GLOW_INTENSITY		= 0.3f;
const float CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY	= 0.05f;

CgFxBloom::CgFxBloom(FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_BLOOM_SHADER, sceneFBO),
sceneSamplerParam(NULL), brightDownSampler2Param(NULL),
brightDownSampler4Param(NULL), brightComposite2Param(NULL), brightComposite4Param(NULL),
highlightThresholdParam(NULL), sceneWidthParam(NULL), 
sceneIntensity(CgFxBloom::DEFAULT_SCENE_INTENSITY), glowIntensity(CgFxBloom::DEFAULT_GLOW_INTENSITY), 
highlightIntensity(CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY), highlightThreshold(CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD),
sceneHeightParam(NULL), sceneIntensityParam(NULL), glowIntensityParam(NULL), highlightIntensityParam(NULL),
bloomFilterFBO(NULL), blurFBO(NULL), downsampleBlur2FBO(NULL),
downsampleBlur4FBO(NULL) {

    //this->SetMenuBloomSettings();

	// Setup the temporary FBOs, used to ping-pong rendering of separable filters and store intermediate renderings
	this->bloomFilterFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), Texture::Bilinear, FBObj::NoAttachment);
	this->blurFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->downsampleBlur2FBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->downsampleBlur4FBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
        sceneFBO->GetFBOTexture()->GetHeight(), Texture::Nearest, FBObj::NoAttachment);

	// Setup any CG parameters
	this->sceneSamplerParam         = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->brightDownSampler2Param	= cgGetNamedEffectParameter(this->cgEffect, "BrightDownsampleLvl2Sampler");
	this->brightDownSampler4Param	= cgGetNamedEffectParameter(this->cgEffect, "BrightDownsampleLvl4Sampler");
	this->brightComposite2Param		= cgGetNamedEffectParameter(this->cgEffect, "BrightCompositeLvl2Sampler");
	this->brightComposite4Param		= cgGetNamedEffectParameter(this->cgEffect, "BrightCompositeLvl4Sampler");
	
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
	delete this->blurFBO;
	this->blurFBO = NULL;
	delete this->downsampleBlur2FBO;
	this->downsampleBlur2FBO = NULL;
	delete this->downsampleBlur4FBO;
	this->downsampleBlur4FBO = NULL;
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

	// Step 2: Using the bright parts of the screen do several down-sampled blurs
	// Setup the down-sampled texture samplers
	cgGLSetTextureParameter(this->brightDownSampler2Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightDownSampler4Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());

	// Do each of the mipmap leveled (down-sampled) bloom blurs
	// Mipmap blur (levels 2, and 4)
	this->DoDownsampledBlur(screenWidth, screenHeight, CgFxBloom::BRIGHT_DOWNSAMPLE_LVL2_TECHNIQUE_NAME, this->brightDownSampler2Param, this->downsampleBlur2FBO);
	this->DoDownsampledBlur(screenWidth, screenHeight, CgFxBloom::BRIGHT_DOWNSAMPLE_LVL4_TECHNIQUE_NAME, this->brightDownSampler4Param, this->downsampleBlur4FBO);

	// Step 3: Final bloom composition
	cgGLSetTextureParameter(this->brightComposite2Param, this->downsampleBlur2FBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightComposite4Param, this->downsampleBlur4FBO->GetFBOTexture()->GetTextureID());
	
	this->sceneFBO->BindFBObj();
	currPass = cgGetFirstPass(this->techniques[CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight, -1.0f);
	cgResetPassState(currPass);

	FBObj::UnbindFBObj();
}

/**
 * Performs a down-sampled blur with the given technique and its corresponding down-sample
 * cg parameter and FBO.
 */
void CgFxBloom::DoDownsampledBlur(int screenWidth, int screenHeight, const std::string& techniqueName, 
                                  CGparameter downsampleParam, FBObj* downsampleFBO) {

	this->blurFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->techniques[techniqueName]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);

	cgGLSetTextureParameter(downsampleParam, this->blurFBO->GetFBOTexture()->GetTextureID());
	
	downsampleFBO->BindFBObj();
	currPass = cgGetNextPass(currPass);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);	
}
