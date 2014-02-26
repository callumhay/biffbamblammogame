/**
 * CgFxSkybox.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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