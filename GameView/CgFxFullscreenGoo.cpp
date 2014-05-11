/**
 * CgFxFullscreenGoo.cpp
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

#include "CgFxFullscreenGoo.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../ResourceManager.h"

const char* CgFxFullscreenGoo::MASK_SPLATTER_TECHNIQUE_NAME		= "MaskSplatter";
const char* CgFxFullscreenGoo::NOMASK_SPLATTER_TECHNIQUE_NAME	= "NoMaskSplatter";

CgFxFullscreenGoo::CgFxFullscreenGoo(FBObj* outputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREENGOO_SHADER, sceneFBO), resultFBO(outputFBO),
timerParam(NULL), scaleParam(NULL), frequencyParam(NULL), displacementParam(NULL), fadeParam(NULL),
colourParam(NULL), noiseSamplerParam(NULL), maskSamplerParam(NULL), sceneSamplerParam(NULL),
scale(2.0f), frequency(0.01f), displacement(0.04f), colour(GameViewConstants::GetInstance()->INK_BLOCK_COLOUR),
maskTex(NULL), timer(0.0f), fade(1.0f) {
	
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxFullscreenGoo::NOMASK_SPLATTER_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->timerParam					= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->scaleParam					= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->frequencyParam			= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->displacementParam		= cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->fadeParam						= cgGetNamedEffectParameter(this->cgEffect, "Fade");
	this->colourParam					= cgGetNamedEffectParameter(this->cgEffect, "Colour");

	// Initialize CG sampler parameters
	this->noiseSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->maskSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "MaskSampler");
	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxFullscreenGoo::~CgFxFullscreenGoo() {
	// Free the mask texture if it exists
	if (this->maskTex != NULL) {
		bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->maskTex);
        UNUSED_VARIABLE(success);
		assert(success);
	}
}

/**
 * Set the mask texture for this effect - causes the technique of the effect to change so that
 * it makes use of the texture found from the given filepath.
 */
void CgFxFullscreenGoo::SetMask(const std::string& texMaskFilepath) {
	if (this->maskTex != NULL) {
		bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->maskTex);
        UNUSED_VARIABLE(success);
		assert(success);
	}
	// Initialize the texture
	this->maskTex	= ResourceManager::GetInstance()->GetImgTextureResource(texMaskFilepath, Texture::Trilinear, GL_TEXTURE_2D);
	assert(this->maskTex != NULL);

	this->currTechnique = this->techniques[CgFxFullscreenGoo::MASK_SPLATTER_TECHNIQUE_NAME];
}

void CgFxFullscreenGoo::Draw(int screenWidth, int screenHeight, double dT) {

	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->frequencyParam, this->frequency);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->fadeParam, this->fade);
	cgGLSetParameter3fv(this->colourParam, this->colour.begin());

	if (this->currTechnique == this->techniques[CgFxFullscreenGoo::MASK_SPLATTER_TECHNIQUE_NAME]) {
		cgGLSetTextureParameter(this->maskSamplerParam, this->maskTex->GetTextureID());
	}
	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
}