/**
 * CgFxBossWeakpoint.cpp
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

#include "CgFxBossWeakpoint.h"
#include "GameViewConstants.h"

const char* CgFxBossWeakpoint::BASIC_TECHNIQUE_NAME    = "Basic";
const char* CgFxBossWeakpoint::TEXTURED_TECHNIQUE_NAME = "Textured";

CgFxBossWeakpoint::CgFxBossWeakpoint() :
CgFxTextureEffectBase(GameViewConstants::GetInstance()->CGFX_BOSSWEAKPOINT_SHADER), 
colourTex(NULL), lightAmt(0.0f) {

    // Make sure the technique is set
    this->SetTexture(NULL);
    
    this->wvpMatrixParam             = cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
    this->colourTexSamplerParam      = cgGetNamedEffectParameter(this->cgEffect, "ColourSampler");
    this->lightAmtParam              = cgGetNamedEffectParameter(this->cgEffect, "LightAmount");
}

CgFxBossWeakpoint::~CgFxBossWeakpoint() {
    this->colourTex = NULL;
}

void CgFxBossWeakpoint::SetTexture(const Texture2D* texture) {
    if (texture != NULL) {
        this->currTechnique = this->techniques[CgFxBossWeakpoint::TEXTURED_TECHNIQUE_NAME];
    }
    else {
        this->currTechnique = this->techniques[CgFxBossWeakpoint::BASIC_TECHNIQUE_NAME];
    }
    this->colourTex = texture;
}

void CgFxBossWeakpoint::SetupBeforePasses(const Camera& camera) {
    UNUSED_PARAMETER(camera);

    cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameter1f(this->lightAmtParam, this->lightAmt);
    
    if (this->colourTex != NULL) {
        cgGLSetTextureParameter(this->colourTexSamplerParam, this->colourTex->GetTextureID());
    }
}