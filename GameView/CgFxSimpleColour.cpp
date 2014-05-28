/**
 * CgFxSimpleColour.cpp
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

#include "CgFxSimpleColour.h"
#include "GameViewConstants.h"

const char* CgFxSimpleColour::BASIC_TECHNIQUE_NAME = "Basic";

CgFxSimpleColour::CgFxSimpleColour() :
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_SIMPLECOLOUR_SHADER), 
wvpMatrixParam(NULL), baseColourParam(NULL), baseColour(0,0,0) {

    this->currTechnique = this->techniques[CgFxSimpleColour::BASIC_TECHNIQUE_NAME];
    this->wvpMatrixParam   = cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
    this->baseColourParam  = cgGetNamedEffectParameter(this->cgEffect, "BaseColour");
}

CgFxSimpleColour::~CgFxSimpleColour() {
}

void CgFxSimpleColour::SetupBeforePasses(const Camera& camera) {
    UNUSED_PARAMETER(camera);

    cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameter3f(this->baseColourParam, this->baseColour.R(), this->baseColour.G(), this->baseColour.B());
}