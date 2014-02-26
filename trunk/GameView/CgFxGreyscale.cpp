/**
 * CgFxGreyscale.cpp
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

#include "CgFxGreyscale.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

const char* CgFxGreyscale::GREYSCALE_TECHNIQUE_NAME = "Greyscale";

CgFxGreyscale::CgFxGreyscale() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_GREYSCALE_SHADER),
colourSamplerParam(NULL), greyscaleFactor(1.0f), colourTexture(NULL) {
    
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxGreyscale::GREYSCALE_TECHNIQUE_NAME];

    // Initialize all of the Cg parameters
    this->greyscaleFactorParam = cgGetNamedEffectParameter(this->cgEffect, "GreyscaleFactor");
    this->colourSamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "ColourSampler");

	debug_cg_state();
}

CgFxGreyscale::~CgFxGreyscale() {
}

void CgFxGreyscale::SetupBeforePasses(const Camera&) {
    assert(this->colourTexture != NULL);
    cgGLSetParameter1f(this->greyscaleFactorParam, this->greyscaleFactor);
    cgGLSetTextureParameter(this->colourSamplerParam, this->colourTexture->GetTextureID());
}