/**
 * CgFxStickyPaddle.cpp
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

#include "CgFxStickyPaddle.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Texture3D.h"

const char* CgFxStickyPaddle::STICKYPADDLE_TECHNIQUE_NAME = "StickyPaddle";

CgFxStickyPaddle::CgFxStickyPaddle(MaterialProperties* properties) :
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_STICKYPADDLE_SHADER, properties),
timerParam(NULL), displacementParam(NULL), speedParam(NULL), waveScaleParam(NULL), refractScaleParam(NULL),
sceneWidthParam(NULL), sceneHeightParam(NULL), sceneSamplerParam(NULL),
timer(0.0f), displacement(0.1f), speed(1.6f), waveScale(3.0f), refractScale(10.0f), noiseScale(3.0f), alpha(1.0f),
sceneTex(NULL) {

	assert(properties->materialType == MaterialProperties::MATERIAL_STICKYGOO_TYPE);
	assert(properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE);

	// Set the technique to the only one available for this effect
	this->currTechnique = this->techniques[CgFxStickyPaddle::STICKYPADDLE_TECHNIQUE_NAME];
	assert(this->currTechnique != NULL);
	
	// Initialize all Cg parameters specific to this shader effect
	this->timerParam           = cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->displacementParam    = cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->speedParam           = cgGetNamedEffectParameter(this->cgEffect, "Speed");
	this->waveScaleParam       = cgGetNamedEffectParameter(this->cgEffect, "WaveScale");
	this->refractScaleParam    = cgGetNamedEffectParameter(this->cgEffect, "RefractScale");
	this->noiseScaleParam      = cgGetNamedEffectParameter(this->cgEffect, "NoiseScale");
	this->sceneWidthParam      = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam     = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");
    this->alphaMultiplierParam = cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");
	this->sceneSamplerParam	   = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	debug_cg_state();
}

CgFxStickyPaddle::~CgFxStickyPaddle() {
}

void CgFxStickyPaddle::SetupBeforePasses(const Camera& camera) {
	CgFxMaterialEffect::SetupBeforePasses(camera);

	this->timer = static_cast<float>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0f;

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->speedParam, this->speed);
	cgGLSetParameter1f(this->waveScaleParam, this->waveScale);
	cgGLSetParameter1f(this->refractScaleParam, this->refractScale);
	cgGLSetParameter1f(this->noiseScaleParam, this->noiseScale);
    cgGLSetParameter1f(this->alphaMultiplierParam, this->alpha);

	// Set the scene texture
	assert(this->sceneTex != NULL);
	cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
	cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

	// Set the noise texture
	cgGLSetTextureParameter(this->texSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
}