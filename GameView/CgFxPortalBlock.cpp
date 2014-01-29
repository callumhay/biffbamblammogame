/**
 * CgFxPortalBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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