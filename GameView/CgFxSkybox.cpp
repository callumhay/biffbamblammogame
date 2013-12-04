/**
 * CgFxSkybox.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxSkybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Texture3D.h"

#include "../ResourceManager.h"

const char* CgFxSkybox::SKYBOX_TECHNIQUE_NAME = "Skybox";

CgFxSkybox::CgFxSkybox(Texture* fgSkyTex, Texture *bgSkyTex) :
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_SKYBOX_SHADER), fgSkyTex(fgSkyTex), bgSkyTex(bgSkyTex),
timer(0.0f), twinkleFreq(0.01f), moveFreq(0.0055f), noiseScale(0.005f), fgScale(1.0f), alpha(1.0f), uvTranslation(0,0) {

    assert(fgSkyTex != NULL);
	assert(bgSkyTex != NULL);

	// Set the technique
	this->currTechnique = this->techniques[SKYBOX_TECHNIQUE_NAME];

	// Transform parameters
	this->wvpMatrixParam   = cgGetNamedEffectParameter(this->cgEffect, "ModelViewProjXf");
	this->worldMatrixParam = cgGetNamedEffectParameter(this->cgEffect, "WorldXf");

	// Noise texture sampler param
	this->fgSkySamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "FGSkySampler");
	this->bgSkySamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "BGSkySampler");

	// Time and frequency parameters
    this->alphaParam       = cgGetNamedEffectParameter(this->cgEffect, "Alpha");
    this->fgScaleParam     = cgGetNamedEffectParameter(this->cgEffect, "FGScale");
	this->timerParam       = cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->twinkleFreqParam = cgGetNamedEffectParameter(this->cgEffect, "TwinkleFreq");
    this->moveFreqParam    = cgGetNamedEffectParameter(this->cgEffect, "MoveFreq"); 
    this->noiseScaleParam  = cgGetNamedEffectParameter(this->cgEffect, "NoiseScale");
	this->viewDirParam     = cgGetNamedEffectParameter(this->cgEffect, "ViewDir");
    this->uvTranslateParam = cgGetNamedEffectParameter(this->cgEffect, "UVTranslate");

	debug_cg_state();
}

CgFxSkybox::~CgFxSkybox() {
	// Release the skybox texture
	bool success = false;
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->fgSkyTex);
    UNUSED_VARIABLE(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bgSkyTex);
    UNUSED_VARIABLE(success);
	assert(success);
}

void CgFxSkybox::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,   CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX,            CG_GL_MATRIX_IDENTITY);

    cgGLSetParameter1f(this->alphaParam, this->alpha);
	cgGLSetParameter1f(this->twinkleFreqParam, this->twinkleFreq);
    cgGLSetParameter1f(this->moveFreqParam, this->moveFreq);
    cgGLSetParameter1f(this->noiseScaleParam, this->noiseScale);
    cgGLSetParameter1f(this->fgScaleParam, this->fgScale);
	double timeInSecs = static_cast<double>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0;
	cgGLSetParameter1f(this->timerParam, timeInSecs);

	Vector3D camDir = camera.GetNormalizedViewVector();
	cgGLSetParameter3fv(this->viewDirParam, camDir.begin());

    cgGLSetParameter2fv(this->uvTranslateParam, this->uvTranslation.begin());

	// Set the fg sky sampler
	cgGLSetTextureParameter(this->fgSkySamplerParam, this->fgSkyTex->GetTextureID());
	// Set the bg sky sampler
	cgGLSetTextureParameter(this->bgSkySamplerParam, this->bgSkyTex->GetTextureID());
}