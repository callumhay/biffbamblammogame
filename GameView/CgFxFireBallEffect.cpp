/**
 * CgFxFireBallEffect.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "CgFxFireBallEffect.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"

const char* CgFxFireBallEffect::BASIC_TECHNIQUE_NAME              = "FireBall";
const char* CgFxFireBallEffect::NO_DEPTH_WITH_MASK_TECHNIQUE_NAME = "FireBallNoDepthWithMask";
const char* CgFxFireBallEffect::PADDLE_ON_FIRE_TECHNIQUE_NAME     = "PaddleOnFire";

CgFxFireBallEffect::CgFxFireBallEffect() : 
CgFxTextureEffectBase(GameViewConstants::GetInstance()->CGFX_FIREBALL_SHADER), 
scale(1.0f), freq(0.6f), flowDir(0, 0, 1), alphaMultiplier(1.0f),
noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()), maskTex(NULL),
brightFireColour(1.0f, 1.0f, 0.0f), darkFireColour(1.0f, 0.0f, 0.0f) {

	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

	// Transform parameters
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");

	// Noise texture sampler param
	this->noiseSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	// Mask texture sampler parameter
	this->maskSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "MaskSampler");

	// Timer paramter
	this->timerParam = cgGetNamedEffectParameter(this->cgEffect, "Timer");

	// Tweakable params
	this->scaleParam            = cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->freqParam             = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->flowDirectionParam    = cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->alphaMultParam        = cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");
	this->brightFireColourParam	= cgGetNamedEffectParameter(this->cgEffect, "BrightFireColour");
	this->darkFireColourParam   = cgGetNamedEffectParameter(this->cgEffect, "DarkFireColour");

	debug_cg_state();
}

CgFxFireBallEffect::~CgFxFireBallEffect() {
}

void CgFxFireBallEffect::SetupBeforePasses(const Camera& camera) {
	UNUSED_PARAMETER(camera);

	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	// Set tweak-ables...
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