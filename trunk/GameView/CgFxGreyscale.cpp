/**
 * CgFxGreyscale.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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