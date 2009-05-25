#include "CgFxGaussianBlur.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../GameController.h"

const std::string CgFxGaussianBlur::GAUSSIANBLUR_3x3_TECHNIQUE_NAME = "GaussianBlur3x3";
const std::string CgFxGaussianBlur::GAUSSIANBLUR_5x5_TECHNIQUE_NAME = "GaussianBlur5x5";
const std::string CgFxGaussianBlur::GAUSSIANBLUR_7x7_TECHNIQUE_NAME = "GaussianBlur7x7";

CgFxGaussianBlur::CgFxGaussianBlur(BlurType blurType, FBObj* sceneFBO) : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_GAUSSIAN_SHADER, sceneFBO),
tempFBO(NULL) {

	// Set the blur type - this will also set the proper technique for the shader
	this->SetBlurType(blurType);

	// Setup the temporary FBO, used to ping-pong rendering of seperable filters
	this->tempFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
														sceneFBO->GetFBOTexture()->GetHeight(), 
														Texture::Nearest,
														FBObj::NoAttachment);

	// Establish all the CG parameters in the effect
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->sceneWidthParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam	= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");
	
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
 * Gaussian blurs are done as 2 seperable filters, the first horizontal
 * and the second vertical (or vice-versa) each filter is a single pass and part of
 * the same technique.
 */
void CgFxGaussianBlur::Draw(int screenWidth, int screenHeight) {
	// Step 0: Establish uniform parameter(s)
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());
	cgGLSetParameter1f(this->sceneWidthParam,  screenWidth);
	cgGLSetParameter1f(this->sceneHeightParam, screenHeight);

	// Step 1: Bind the temporary FBO and draw a fullscreen quad with the first pass of the effect.
	this->tempFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->tempFBO->UnbindFBObj();

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