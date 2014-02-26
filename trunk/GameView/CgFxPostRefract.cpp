/**
 * CgFxPostRefract.cpp
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

#include "CgFxPostRefract.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxPostRefract::BASIC_TECHNIQUE_NAME						= "PostRefractGeom";
const char* CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME				= "PostRefractNormalTex";
const char* CgFxPostRefract::NORMAL_TEXTURE_WITH_OVERLAY_TECHNIQUE_NAME	= "PostRefractNormalTexWithOverlay";
const char* CgFxPostRefract::NORMAL_TEXTURE_WITH_NOISE_TECHNIQUE_NAME	= "PostRefractNormalTexWithNoise";

// Default constructor
CgFxPostRefract::CgFxPostRefract() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_POSTREFRACT_SHADER), 
indexOfRefraction(1.00f), warpAmount(1.0f), sceneTex(NULL), normalTex(NULL), overlayTex(NULL),
timer(0.0), scale(0.05f), freq(1.0f) {
	
	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

	// Transform parameters
	this->worldITMatrixParam  = cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
	this->wvpMatrixParam      = cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam    = cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	this->viewInvMatrixParam  = cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

	// Refraction parameters
	this->indexOfRefactionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam       = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam      = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	// The rendered scene background texture
	this->sceneSamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->normalSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "NormalSampler");
	this->overlaySamplerParam = cgGetNamedEffectParameter(this->cgEffect, "OverlaySampler");
	this->noiseSamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");

	this->noiseScaleParam = cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->noiseFreqParam  = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->timerParam      = cgGetNamedEffectParameter(this->cgEffect, "Timer");

	debug_cg_state();
}

CgFxPostRefract::~CgFxPostRefract() {
}

void CgFxPostRefract::SetupBeforePasses(const Camera& camera) {

	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,     CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam,   CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

	const Matrix4x4& invViewXf = camera.GetInvViewTransform();
	cgGLSetMatrixParameterfc(this->viewInvMatrixParam, invViewXf.begin());

	// Refraction property setup
	cgGLSetParameter1f(this->indexOfRefactionParam, this->indexOfRefraction);
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Timer, scale and frequency for noise...
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->noiseScaleParam, this->scale);
	cgGLSetParameter1f(this->noiseFreqParam, this->freq);

	// Set the scene texture
	if (this->sceneTex != NULL) {
		cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
		cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
		cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());
	}
	// If there's a normal texture, set it
	if (this->normalTex != NULL) {
		cgGLSetTextureParameter(this->normalSamplerParam, this->normalTex->GetTextureID());
	}
	if (this->overlayTex != NULL) {
		cgGLSetTextureParameter(this->overlaySamplerParam, this->overlayTex->GetTextureID());
	}
	// Set the noise texture sampler
	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
}