#include "CgFxVolumetricEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgShaderManager.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const std::string CgFxVolumetricEffect::BASIC_TECHNIQUE_NAME				= "Basic";
const std::string CgFxVolumetricEffect::GHOSTBALL_TECHNIQUE_NAME		= "GhostBall";
const std::string CgFxVolumetricEffect::SMOKESPRITE_TECHNIQUE_NAME	= "SmokeSprite";
const std::string CgFxVolumetricEffect::FIRESPRITE_TECHNIQUE_NAME		= "FireSprite";

// Default constructor
CgFxVolumetricEffect::CgFxVolumetricEffect() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_VOLUMEMETRIC_SHADER), 
scale(1.0f), freq(1.0f), fadeExponent(1),
colour(1,1,1), flowDir(0, 0, 1), constAmt(0.0f), 
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
	this->constAmtParam				= cgGetNamedEffectParameter(this->cgEffect, "ConstAmount");
	this->alphaMultParam			= cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");

	CgShaderManager::Instance()->CheckForCgError("Getting parameters for CgFxPostRefract");
}

CgFxVolumetricEffect::~CgFxVolumetricEffect() {
}

void CgFxVolumetricEffect::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	// Obtain the current model view and inverse view transforms
	float tempMVXfVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	Matrix4x4 tempMVXf(tempMVXfVals);
	Matrix4x4 invViewXf = camera.GetInvViewTransform();

	// Make sure that JUST the world transform is set
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invViewXf.begin());
	// Set the inverse view transform parameter
	cgGLSetStateMatrixParameter(this->viewInvMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glMultMatrixf(tempMVXf.begin());
	// Set the world transform parameters
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();

	// Set tweakables...
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->freqParam, this->freq);
	cgGLSetParameter1f(this->fadeExpParam, this->fadeExponent);
	cgGLSetParameter1f(this->constAmtParam, this->constAmt);
	cgGLSetParameter1f(this->alphaMultParam, this->alphaMultiplier);
	cgGLSetParameter1f(this->timerParam, static_cast<float>(this->timer));
	cgGLSetParameter3f(this->flowDirectionParam, this->flowDir[0], this->flowDir[1], this->flowDir[2]);
	cgGLSetParameter3f(this->colourParam, this->colour[0], this->colour[1], this->colour[2]);

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