/**
 * CgFxCelOutlines.cpp
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

#include "CgFxCelOutlines.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

const char* CgFxCelOutlines::TECHNIQUE_NAME = "CelOutlines";

CgFxCelOutlines::CgFxCelOutlines() : 
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_CELOUTLINES_SHADER, NULL),
nearZParam(NULL), farZParam(NULL), texOffsetsParam(NULL), colourSamplerParam(NULL), depthSamplerParam(NULL),
minDistance(0.001f), maxDistance(30.0f), contrastExp(1.0f), alphaMultiplier(1.0f), offsetMultiplier(1.0f), 
ambientBrightness(1.0f) {
    
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxCelOutlines::TECHNIQUE_NAME];

    // Initialize all of the Cg parameters
    this->nearZParam         = cgGetNamedEffectParameter(this->cgEffect, "Near");
    this->farZParam          = cgGetNamedEffectParameter(this->cgEffect, "Far");
    this->texOffsetsParam    = cgGetNamedEffectParameter(this->cgEffect, "TextureOffsets");

    this->minDistanceParam = cgGetNamedEffectParameter(this->cgEffect, "MinDistance");
    this->maxDistanceParam = cgGetNamedEffectParameter(this->cgEffect, "MaxDistance");

    this->ambientBrightnessParam = cgGetNamedEffectParameter(this->cgEffect, "AmbientBrightness");
    this->ambientDarknessParam = cgGetNamedEffectParameter(this->cgEffect, "AmbientDarkness");

    this->contrastParam = cgGetNamedEffectParameter(this->cgEffect, "Contrast");

    this->colourSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "ColourSampler");
    this->depthSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "DepthSampler");

    this->alphaMultiplierParam = cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");

	debug_cg_state();
}

CgFxCelOutlines::~CgFxCelOutlines() {
}

void CgFxCelOutlines::Draw(FBObj* colourAndDepthSceneFBO, FBObj* outputFBO) {
    assert(colourAndDepthSceneFBO != NULL);
    assert(outputFBO != NULL);

    cgGLSetParameter1f(this->nearZParam, Camera::NEAR_PLANE_DIST);
    cgGLSetParameter1f(this->farZParam, Camera::FAR_PLANE_DIST);
    cgGLSetParameter1f(this->minDistanceParam, this->minDistance);
    cgGLSetParameter1f(this->maxDistanceParam, this->maxDistance);
    cgGLSetParameter1f(this->ambientBrightnessParam, this->ambientBrightness);
    cgGLSetParameter1f(this->ambientDarknessParam, 0.1f);
    cgGLSetParameter1f(this->contrastParam, this->contrastExp);
    cgGLSetParameter1f(this->alphaMultiplierParam, this->alphaMultiplier);

    int renderWidth  = colourAndDepthSceneFBO->GetFBOTexture()->GetWidth();
    int renderHeight = colourAndDepthSceneFBO->GetFBOTexture()->GetHeight();

    // Texture offsets...
	// Layout is as follows, where x the middle is the "non-offset" fragment (index = 4)
	// 0 1 2
	// 3 x 5
	// 6 7 8
    float dX = this->offsetMultiplier / static_cast<float>(renderWidth);
    float dY = this->offsetMultiplier / static_cast<float>(renderHeight);
    
    float offsets[18];
    
    // Top row
    offsets[0] = -dX;  offsets[1] = dY; // UV20
    offsets[2] = 0.0f; offsets[3] = dY; // UV21
    offsets[4] = dX;   offsets[5] = dY; // UV22
    // Middle row
    offsets[6]  = -dX;  offsets[7]  = 0.0f; // UV10
    offsets[8]  = 0.0f; offsets[9]  = 0.0f; // UV11
    offsets[10] = dX;   offsets[11] = 0.0f; // UV12
    // Bottom row
    offsets[12] = -dX;  offsets[13] = -dY; // UV00
    offsets[14] = 0.0f; offsets[15] = -dY; // UV01
    offsets[16] = dX;   offsets[17] = -dY; // UV02

    cgGLSetParameterArray2f(this->texOffsetsParam, 0, 9, offsets);

    // Set the texture samplers to those in the input FBO
    cgGLSetTextureParameter(this->colourSamplerParam, colourAndDepthSceneFBO->GetFBOTexture()->GetTextureID());
    assert(colourAndDepthSceneFBO->GetRenderToDepthTexture() != NULL);
    cgGLSetTextureParameter(this->depthSamplerParam, colourAndDepthSceneFBO->GetRenderToDepthTexture()->GetTextureID()); 

	// Draw a fullscreen quad with the effect applied and draw it all into the output FBO
	outputFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(renderWidth, renderHeight);
	cgResetPassState(currPass);
	outputFBO->UnbindFBObj();
}

