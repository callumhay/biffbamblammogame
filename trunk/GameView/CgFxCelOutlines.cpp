/**
 * CgFxCelOutlines.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

