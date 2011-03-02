/**
 * CgFxDecoSkybox.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxDecoSkybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Texture3D.h"

#include "../ResourceManager.h"

const char* CgFxDecoSkybox::DECOSKYBOX_TECHNIQUE_NAME = "DecoSkybox";

CgFxDecoSkybox::CgFxDecoSkybox(Texture *skyTex) :
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_DECOSKYBOX_SHADER), skyTex(skyTex),
timer(0.0f), freq(0.025f), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {
	assert(skyTex != NULL);

	// Set the technique
	this->currTechnique = this->techniques[DECOSKYBOX_TECHNIQUE_NAME];

	// Transform parameters
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "ModelViewProjXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");

	// Noise texture sampler param
	this->noiseSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->skySamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "SkySampler");

	// Time and frequency paramters
	this->timerParam = cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->freqParam	 = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->viewDirParam = cgGetNamedEffectParameter(this->cgEffect, "ViewDir");

	debug_cg_state();
}

CgFxDecoSkybox::~CgFxDecoSkybox() {
	// Release the skybox texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->skyTex);
	assert(success);
}

void CgFxDecoSkybox::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,   CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX,            CG_GL_MATRIX_IDENTITY);

	cgGLSetParameter1f(this->freqParam, this->freq);
	double timeInSecs = static_cast<double>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0;
	cgGLSetParameter1f(this->timerParam, timeInSecs);

	Vector3D camDir = camera.GetNormalizedViewVector();
	cgGLSetParameter3fv(this->viewDirParam, camDir.begin());

	// Set noise sampler
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);
	// Set the sky sampler
	cgGLSetTextureParameter(this->skySamplerParam, this->skyTex->GetTextureID());
}