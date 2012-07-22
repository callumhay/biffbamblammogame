/**
 * CgFxGaussianBlur.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
 * the same technique. Note that the extra parameter is used to add an extra
 * blur effect - kind of like being drunk, if needed.
 */
void CgFxGaussianBlur::Draw(int screenWidth, int screenHeight, double dT) {
	
	float revisedWidth  = screenWidth;
	float revisedHeight = screenHeight;
	
	// Check for any animations
	std::map<BlurAnimations, AnimationMultiLerp<float> >::iterator animWidthFind = this->blurAnims.find(PoisonWidthBlurAnimation);
	if (animWidthFind != this->blurAnims.end()) {
		revisedWidth = animWidthFind->second.GetInterpolantValue();
		bool isDone = animWidthFind->second.Tick(dT);
		if (isDone) {
			this->blurAnims.erase(animWidthFind);
		}
	}

	// Step 0: Establish uniform parameter(s)
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());
	cgGLSetParameter1f(this->sceneWidthParam,  revisedWidth);
	cgGLSetParameter1f(this->sceneHeightParam, revisedHeight);

	// Step 1: Bind the temporary FBO and draw a fullscreen quad with the first pass of the effect.
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

		AnimationMultiLerp<float> blurWidthAnim;
		blurWidthAnim.SetLerp(timeVals, blurVals);
		blurWidthAnim.SetRepeat(true);

		this->blurAnims[PoisonWidthBlurAnimation] = blurWidthAnim;
	}
	else {
		// Grab the already existing (previously active) poison animations
		std::map<BlurAnimations, AnimationMultiLerp<float> >::iterator animWidthFind = this->blurAnims.find(PoisonWidthBlurAnimation);
		assert(animWidthFind  != this->blurAnims.end());
	
		// Fade back to normal and take off repeat/looping so that the animation dies when complete
		timeVals.reserve(2);
		timeVals.push_back(0.0f);
		timeVals.push_back(3.0f);
		
		blurVals.reserve(2);
		blurVals.push_back(animWidthFind->second.GetInterpolantValue());
		blurVals.push_back(this->sceneFBO->GetFBOTexture()->GetWidth());

		animWidthFind->second.SetLerp(timeVals, blurVals);
		animWidthFind->second.SetRepeat(false);	
	}
}