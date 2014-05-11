/**
 * CgFxPortalBlock.cpp
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

#include "CgFxPortalBlock.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxPortalBlock::PORTAL_BLOCK_TECHNIQUE_NAME = "PortalBlock";

// Default constructor
CgFxPortalBlock::CgFxPortalBlock() : 
CgFxAbstractMaterialEffect(GameViewConstants::GetInstance()->CGFX_PORTALBLOCK_SHADER), 
alphaMultiplier(1.0f), indexOfRefraction(1.55f), scale(0.05f), freq(1.0f), flowDir(0, 0, 1),
timer(0.0), warpAmount(60.0f), sceneTex(NULL), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {

	// Set the technique
	this->currTechnique = this->techniques[PORTAL_BLOCK_TECHNIQUE_NAME];

    // Transform parameters
    this->worldITMatrixParam = cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
    this->wvpMatrixParam     = cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
    this->worldMatrixParam   = cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
    this->viewInvMatrixParam = cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

    // Set basic parameters
    this->alphaMultiplierParam  = cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");

	// Set refraction parameters
	this->indexOfRefactionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam       = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam      = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	this->noiseScaleParam   = cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->noiseFreqParam    = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->noiseFlowDirParam = cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->timerParam        = cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->noiseSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");

	// The rendered scene background texture
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	debug_cg_state();
}

CgFxPortalBlock::~CgFxPortalBlock() {
}

void CgFxPortalBlock::SetupBeforePasses(const Camera& camera) {

    // Transforms
    cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

    // Obtain the current model view and inverse view transforms
    float tempMVXfVals[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);

    const Matrix4x4& invViewXf = camera.GetInvViewTransform();
    cgGLSetMatrixParameterfc(this->viewInvMatrixParam, invViewXf.begin());

    // Make sure that JUST the world transform is set
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(invViewXf.begin());
    glMultMatrixf(tempMVXfVals);

    // Set the world transform parameters
    cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
    cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    glPopMatrix();    

    cgGLSetParameter1f(this->alphaMultiplierParam, this->alphaMultiplier);

	// Refraction property setup
	cgGLSetParameter1f(this->indexOfRefactionParam, this->indexOfRefraction);
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Set the scene texture
	cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
	cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());
	assert(this->sceneTex != NULL);
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->noiseScaleParam, this->scale);
	cgGLSetParameter1f(this->noiseFreqParam, this->freq);
	cgGLSetParameter3fv(this->noiseFlowDirParam, this->flowDir.begin());
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);
}