#include "CgFxBloom.h"
#include "GameViewConstants.h"

#include "../GameController.h"

#include "../BlammoEngine/FBObj.h"

const std::string CgFxBloom::BLOOM_FILTER_TECHNIQUE_NAME						= "BloomFilter";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL2_TECHNIQUE_NAME	= "BrightDownsampleBlur2";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL3_TECHNIQUE_NAME	= "BrightDownsampleBlur3";
const std::string CgFxBloom::BRIGHT_DOWNSAMPLE_LVL4_TECHNIQUE_NAME	= "BrightDownsampleBlur4";
const std::string CgFxBloom::BLOOM_COMPOSITION_TECHNIQUE_NAME				= "CompositeBloom";

const float CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD	= 0.55f;
const float CgFxBloom::DEFAULT_SCENE_INTENSITY			= 0.85f;
const float CgFxBloom::DEFAULT_GLOW_INTENSITY				= 0.3f;
const float CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY	= 0.2f;

CgFxBloom::CgFxBloom(FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_BLOOM_SHADER, sceneFBO),
sceneSamplerParam(NULL), brightDownSampler2Param(NULL), brightDownSampler3Param(NULL), 
brightDownSampler4Param(NULL), brightComposite2Param(NULL), brightComposite3Param(NULL), brightComposite4Param(NULL),
highlightThresholdParam(NULL), sceneWidthParam(NULL), 
sceneIntensity(CgFxBloom::DEFAULT_SCENE_INTENSITY), 
glowIntensity(CgFxBloom::DEFAULT_GLOW_INTENSITY), 
highlightIntensity(CgFxBloom::DEFAULT_HIGHLIGHT_INTENSITY), 
highlightThreshold(CgFxBloom::DEFAULT_HIGHLIGHT_THRESHOLD),
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

void CgFxBloom::Draw(int screenWidth, int screenHeight, double dT) {
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

/**
 * Obtain an animation that will pulse this bloom effect, where
 * each pulse will be of the given length of time in seconds and with
 * a magnitude (strength of pulse) of the given amount.
 * Returns: Animation for the bloom that pulses it (gives a kind of sickly feeling).
 */
std::list<AnimationMultiLerp<float>> CgFxBloom::GetPulseAnimation(float pulseLengthInSec, float pulseAmount) {
	assert(pulseLengthInSec > 0);
	assert(pulseAmount > 0);

	// Set up the animation for the highlight intensity - this will pulse the intensity of the highlight of this
	// bloom effect over the given time of pulseLengthInSec
	AnimationMultiLerp<float> pulseHighlightAnim(&this->highlightIntensity);

	std::vector<float> intensityValues;
	intensityValues.reserve(5);
	intensityValues.push_back(this->highlightIntensity);
	intensityValues.push_back(this->highlightIntensity + pulseAmount);
	intensityValues.push_back(this->highlightIntensity);
	intensityValues.push_back(this->highlightIntensity - pulseAmount);
	intensityValues.push_back(this->highlightIntensity);
	
	std::vector<double> timeValues1;
	timeValues1.reserve(5);
	timeValues1.push_back(0.0);
	timeValues1.push_back(pulseLengthInSec);
	timeValues1.push_back(2.0 * pulseLengthInSec);
	timeValues1.push_back(3.0 * pulseLengthInSec);
	timeValues1.push_back(4.0 * pulseLengthInSec);

	pulseHighlightAnim.SetLerp(timeValues1, intensityValues);
	pulseHighlightAnim.SetRepeat(true);

	// Setup the animation for the glow intensity - similar to the above but for glow intensity parameter.
	AnimationMultiLerp<float> pulseGlowAnim(&this->glowIntensity);

	std::vector<float> glowValues;
	glowValues.reserve(5);
	glowValues.push_back(this->glowIntensity);
	glowValues.push_back(this->glowIntensity + pulseAmount);
	glowValues.push_back(this->glowIntensity);
	glowValues.push_back(this->glowIntensity - pulseAmount);
	glowValues.push_back(this->glowIntensity);
	
	std::vector<double> timeValues2;
	timeValues2.reserve(5);
	timeValues2.push_back(0.0);
	timeValues2.push_back(pulseLengthInSec);
	timeValues2.push_back(2.0 * pulseLengthInSec);
	timeValues2.push_back(3.0 * pulseLengthInSec);
	timeValues2.push_back(4.0 * pulseLengthInSec);

	pulseGlowAnim.SetLerp(timeValues2, glowValues);
	pulseGlowAnim.SetRepeat(true);	

	std::list<AnimationMultiLerp<float>> retVal;
	retVal.push_back(pulseHighlightAnim);
	retVal.push_back(pulseGlowAnim);

	return retVal;
}