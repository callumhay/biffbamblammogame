/**
 * CgFxBossWeakpoint.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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