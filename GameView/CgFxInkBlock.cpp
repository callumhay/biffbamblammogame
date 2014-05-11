/**
 * CgFxInkBlock.cpp
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

#include "CgFxInkBlock.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"

const char* CgFxInkBlock::INKBLOCK_TECHNIQUE_NAME = "InkBlock";

CgFxInkBlock::CgFxInkBlock(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_INKBLOCK_SHADER, properties),
timerParam(NULL), displacementParam(NULL), sharpnessParam(NULL), speedParam(NULL), turbDensityParam(NULL), 
timer(0.0f), displacement(1.6f), sharpness(1.5f), speed(0.35f), turbulanceDensity(1.2f) {
	
	assert(properties->materialType == MaterialProperties::MATERIAL_INKBLOCK_TYPE);
	assert(properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE);

	// Set the technique to the only one available for this effect
	this->currTechnique = this->techniques[CgFxInkBlock::INKBLOCK_TECHNIQUE_NAME];
	assert(this->currTechnique != NULL);

	// Initialize any other parameters specific to this shader effect
	this->timerParam				= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->displacementParam	= cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->sharpnessParam		= cgGetNamedEffectParameter(this->cgEffect, "Sharpness");
	this->speedParam				= cgGetNamedEffectParameter(this->cgEffect, "Speed");
	this->turbDensityParam	= cgGetNamedEffectParameter(this->cgEffect, "TurbDensity");

	debug_cg_state();
}

CgFxInkBlock::~CgFxInkBlock() {
}

/**
 * Setup all shader specific parameters and samplers before any
 * techniques / passes are executed.
 */
void CgFxInkBlock::SetupBeforePasses(const Camera& camera) {
	CgFxMaterialEffect::SetupBeforePasses(camera);

	this->timer = static_cast<float>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0f;

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->sharpnessParam, this->sharpness);
	cgGLSetParameter1f(this->speedParam, this->speed);
	cgGLSetParameter1f(this->turbDensityParam, this->turbulanceDensity);

	// Set the colour texture sampler to be noise
	cgGLSetTextureParameter(this->texSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
}