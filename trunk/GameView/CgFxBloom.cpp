#include "CgFxBloom.h"
#include "GameViewConstants.h"

#include "../GameController.h"

#include "../BlammoEngine/FBObj.h"

const std::string CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME						= "BloomFilter";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL2_TECHNIQUE_NAME	= "BrightDownsampleBlur2";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL3_TECHNIQUE_NAME	= "BrightDownsampleBlur3";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL4_TECHNIQUE_NAME	= "BrightDownsampleBlur4";
const std::string CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME				= "CompositeBloom";

CgFxBloom::CgFxBloom(FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_BLOOM_SHADER, sceneFBO),
sceneSamplerParam(NULL), brightDownSampler2Param(NULL), brightDownSampler3Param(NULL), 
brightDownSampler4Param(NULL), brightComposite2Param(NULL), brightComposite3Param(NULL), brightComposite4Param(NULL),
highlightThresholdParam(NULL), sceneWidthParam(NULL), 
sceneIntensity(0.85f), glowIntensity(0.3f), highlightIntensity(0.2f), highlightThreshold(0.55f),
sceneHeightParam(NULL), sceneIntensityParam(NULL), glowIntensityParam(NULL), highlightIntensityParam(NULL),
bloomFilterFBO(NULL), blurFBO(NULL), downsampleBlur2FBO(NULL),
downsampleBlur3FBO(NULL), downsampleBlur4FBO(NULL) {

	// Setup the temporary FBOs, used to ping-pong rendering of seperable filters and store intermediate renderings
	this->bloomFilterFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Bilinear,
														FBObj::NoAttachment);

	this->blurFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Nearest,
														FBObj::NoAttachment);

	this->downsampleBlur2FBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Nearest,
														FBObj::NoAttachment);
	this->downsampleBlur3FBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Nearest,
														FBObj::NoAttachment);
	this->downsampleBlur4FBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Nearest,
														FBObj::NoAttachment);

	// Setup any CG parameters
	this->sceneSamplerParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->brightDownSampler2Param	= cgGetNamedEffectParameter(this->cgEffect, "BrightDownsampleLvl2Sampler");
	this->brightDownSampler3Param	= cgGetNamedEffectParameter(this->cgEffect, "BrightDownsampleLvl3Sampler");
	this->brightDownSampler4Param	= cgGetNamedEffectParameter(this->cgEffect, "BrightDownsampleLvl4Sampler");
	this->brightComposite2Param		= cgGetNamedEffectParameter(this->cgEffect, "BrightCompositeLvl2Sampler");
	this->brightComposite3Param		= cgGetNamedEffectParameter(this->cgEffect, "BrightCompositeLvl3Sampler");
	this->brightComposite4Param		= cgGetNamedEffectParameter(this->cgEffect, "BrightCompositeLvl4Sampler");
	
	this->sceneWidthParam					= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	this->highlightThresholdParam	= cgGetNamedEffectParameter(this->cgEffect, "HighlightThreshold");
	this->sceneIntensityParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneIntensity");
	this->glowIntensityParam			= cgGetNamedEffectParameter(this->cgEffect, "GlowIntensity");
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
	delete this->downsampleBlur3FBO;
	this->downsampleBlur3FBO = NULL;
	delete this->downsampleBlur4FBO;
	this->downsampleBlur4FBO = NULL;
}

void CgFxBloom::Draw(int screenWidth, int screenHeight) {
	// Step 0: Setup necessary cg parameters
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	cgGLSetParameter1f(this->sceneWidthParam,  screenWidth);
	cgGLSetParameter1f(this->sceneHeightParam, screenHeight);

	cgGLSetParameter1f(this->highlightThresholdParam, this->highlightThreshold);
	cgGLSetParameter1f(this->sceneIntensityParam,			this->sceneIntensity);
	cgGLSetParameter1f(this->glowIntensityParam,			this->glowIntensity);
	cgGLSetParameter1f(this->highlightIntensityParam, this->highlightIntensity);

	// Step 1: Draw a full screen quad with the bloom filter applied to it
	// render it into a temporary FBO - this will grab all the very bright parts
	// of the screen
	this->bloomFilterFBO->BindFBObj();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CGpass currPass = cgGetFirstPass(this->techniques[CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->bloomFilterFBO->UnbindFBObj();

	// Step 2: Using the bright parts of the screen do several downsampled blurs
	// Setup the downsampled texture samplers
	cgGLSetTextureParameter(this->brightDownSampler2Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightDownSampler3Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightDownSampler4Param, this->bloomFilterFBO->GetFBOTexture()->GetTextureID());

	// Do each of the mipmap leveled (downsampled) bloom blurs
	// Mipmap blur (levels 2, 3, 4)
	this->DoDownsampledBlur(screenWidth, screenHeight, CgFxBloom::BRIGHT_DOWNSAMPLE_LVL2_TECHNIQUE_NAME, this->brightDownSampler2Param, this->downsampleBlur2FBO);
	this->DoDownsampledBlur(screenWidth, screenHeight, CgFxBloom::BRIGHT_DOWNSAMPLE_LVL3_TECHNIQUE_NAME, this->brightDownSampler3Param, this->downsampleBlur3FBO);
	this->DoDownsampledBlur(screenWidth, screenHeight, CgFxBloom::BRIGHT_DOWNSAMPLE_LVL4_TECHNIQUE_NAME, this->brightDownSampler4Param, this->downsampleBlur4FBO);

	// Step 3: Final bloom composition
	cgGLSetTextureParameter(this->brightComposite2Param, this->downsampleBlur2FBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightComposite3Param, this->downsampleBlur3FBO->GetFBOTexture()->GetTextureID());
	cgGLSetTextureParameter(this->brightComposite4Param, this->downsampleBlur4FBO->GetFBOTexture()->GetTextureID());
	
	this->sceneFBO->BindFBObj();
	currPass = cgGetFirstPass(this->techniques[CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight, -1.0f);
	cgResetPassState(currPass);
	this->sceneFBO->UnbindFBObj();
}

/**
 * Performs a downsampled blur with the given technique and its corresponding downsample
 * cg parameter and FBO.
 */
void CgFxBloom::DoDownsampledBlur(int screenWidth, int screenHeight, const std::string& techniqueName, CGparameter downsampleParam, FBObj* downsampleFBO) {
	this->blurFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->techniques[techniqueName]);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->blurFBO->UnbindFBObj();

	cgGLSetTextureParameter(downsampleParam, this->blurFBO->GetFBOTexture()->GetTextureID());
	
	downsampleFBO->BindFBObj();
	currPass = cgGetNextPass(currPass);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);	
	downsampleFBO->UnbindFBObj();
}