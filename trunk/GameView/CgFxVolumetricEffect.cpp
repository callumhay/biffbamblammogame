/**
 * CgFxVolumetricEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxVolumetricEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxVolumetricEffect::BASIC_TECHNIQUE_NAME				= "Basic";
const char* CgFxVolumetricEffect::GHOSTBALL_TECHNIQUE_NAME		= "GhostBall";
const char* CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME	= "SmokeSprite";
const char* CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME		= "FireSprite";

// Default constructor
CgFxVolumetricEffect::CgFxVolumetricEffect() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_VOLUMEMETRIC_SHADER), 
scale(1.0f), freq(1.0f), fadeExponent(1), colour(1,1,1), flowDir(0, 0, 1), alphaMultiplier(1.0f), 
noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()), maskTex(NULL) {

	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

	// Transform parameters
	this->worldITMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	this->viewInvMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

	// Noise texture sampler param
	this->noiseSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->maskSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "MaskSampler");

	// Timer paramter
	this->timerParam = cgGetNamedEffectParameter(this->cgEffect, "Timer");

	// Tweakable params
	this->scaleParam					= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->freqParam						= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->flowDirectionParam	= cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->colourParam					= cgGetNamedEffectParameter(this->cgEffect, "Colour");
	this->fadeExpParam				= cgGetNamedEffectParameter(this->cgEffect, "FadeExp");
	this->alphaMultParam			= cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");

	debug_cg_state();
}

CgFxVolumetricEffect::~CgFxVolumetricEffect() {
}

void CgFxVolumetricEffect::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,     CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam,   CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	const Matrix4x4& invViewXf = camera.GetInvViewTransform();
	cgGLSetMatrixParameterfc(this->viewInvMatrixParam, invViewXf.begin());

	// Set tweakables...
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->freqParam, this->freq);
	cgGLSetParameter1f(this->fadeExpParam, this->fadeExponent);
	cgGLSetParameter1f(this->alphaMultParam, this->alphaMultiplier);
	cgGLSetParameter3fv(this->flowDirectionParam, this->flowDir.begin());
	cgGLSetParameter3fv(this->colourParam, this->colour.begin());

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