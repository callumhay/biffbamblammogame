/**
 * CgFxPostDeath.cpp
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

#include "CgFxPostDeath.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"

const char* CgFxPostDeath::POSTDEATH_TECHNIQUE_NAME = "PostDeath";

CgFxPostDeath::CgFxPostDeath(FBObj* inputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREEN_POSTDEATH_SHADER, inputFBO),
intensityParam(NULL), alphaParam(NULL), sceneSamplerParam(NULL), intensity(0.0f), alpha(1.0f) {

	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostDeath::POSTDEATH_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->intensityParam = cgGetNamedEffectParameter(this->cgEffect, "Intensity");
    this->alphaParam     = cgGetNamedEffectParameter(this->cgEffect, "Alpha");

	// Initialize CG sampler parameters
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxPostDeath::~CgFxPostDeath() {
}

void CgFxPostDeath::Draw(int screenWidth, int screenHeight, double dT) {
    UNUSED_PARAMETER(dT);

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->intensityParam, this->intensity);
    cgGLSetParameter1f(this->alphaParam, this->alpha);

	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
    GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight, -1.0f);
	cgResetPassState(currPass);
}