/**
 * CgFxFireBallEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxFireBallEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"

const char* CgFxFireBallEffect::BASIC_TECHNIQUE_NAME					= "FireBall";
const char* CgFxFireBallEffect::NO_DEPTH_WITH_MASK_TECHNIQUE_NAME	= "FireBallNoDepthWithMask";

CgFxFireBallEffect::CgFxFireBallEffect() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_FIREBALL_SHADER), 
scale(1.0f), freq(0.6f), flowDir(0, 0, 1), alphaMultiplier(1.0f),
noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()), maskTex(NULL),
brightFireColour(1.0f, 1.0f, 0.0f), darkFireColour(1.0f, 0.0f, 0.0f) {

	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

	// Transform parameters
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");

	// Noise texture sampler param
	this->noiseSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	// Mask texture sampler parameter
	this->maskSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "MaskSampler");

	// Timer paramter
	this->timerParam = cgGetNamedEffectParameter(this->cgEffect, "Timer");

	// Tweakable params
	this->scaleParam						= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->freqParam							= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->flowDirectionParam		= cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->alphaMultParam				= cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");
	this->brightFireColourParam	= cgGetNamedEffectParameter(this->cgEffect, "BrightFireColour");
	this->darkFireColourParam		= cgGetNamedEffectParameter(this->cgEffect, "DarkFireColour");

	debug_cg_state();
}

CgFxFireBallEffect::~CgFxFireBallEffect() {
}

void CgFxFireBallEffect::SetupBeforePasses(const Camera& camera) {
	UNUSED_PARAMETER(camera);

	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,   CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

	// Set tweakables...
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->freqParam, this->freq);
	cgGLSetParameter1f(this->alphaMultParam, this->alphaMultiplier);
	cgGLSetParameter3fv(this->flowDirectionParam, this->flowDir.begin());
	cgGLSetParameter3fv(this->brightFireColourParam, this->brightFireColour.begin());
	cgGLSetParameter3fv(this->darkFireColourParam, this->darkFireColour.begin());

	// Set the timer and noise parameters...
	double timeInSecs = static_cast<double>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0;
	cgGLSetParameter1f(this->timerParam, timeInSecs);

	// Set noise texture sampler...
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);

	// Set the mask texture sampler if it exists
	if (this->maskTex != NULL) {
		cgGLSetTextureParameter(this->maskSamplerParam, this->maskTex->GetTextureID());
	}
}