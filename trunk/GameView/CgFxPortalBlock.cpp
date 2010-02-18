#include "CgFxPortalBlock.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxPortalBlock::PORTAL_BLOCK_TECHNIQUE_NAME = "PortalBlock";

// Default constructor
CgFxPortalBlock::CgFxPortalBlock(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PORTALBLOCK_SHADER, properties), 
indexOfRefraction(1.00f), scale(1.0f), freq(1.0f), flowDir(0, 0, 1),
timer(0.0), warpAmount(1.0f), sceneTex(NULL), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {
	
	// Set the technique
	this->currTechnique = this->techniques[PORTAL_BLOCK_TECHNIQUE_NAME];

	// Transform parameters
	this->worldITMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	this->viewInvMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

	// Refraction parameters
	this->indexOfRefactionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam			  = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	// Vapour trail parameters
	this->noiseScaleParam				= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->noiseFreqParam				= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->noiseFlowDirParam			= cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->timerParam						= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->noiseSamplerParam			= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");

	// The rendered scene background texture
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	debug_cg_state();
}

CgFxPortalBlock::~CgFxPortalBlock() {
}

void CgFxPortalBlock::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	// Obtain the current model view and inverse view transforms
	float tempMVXfVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	Matrix4x4 tempMVXf(tempMVXfVals);
	Matrix4x4 invViewXf = camera.GetInvViewTransform();

	// Make sure that JUST the world transform is set
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invViewXf.begin());
	// Set the inverse view transform parameter
	cgGLSetStateMatrixParameter(this->viewInvMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glMultMatrixf(tempMVXf.begin());
	// Set the world transform parameters
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();

	// Refraction property setup
	cgGLSetParameter1f(this->indexOfRefactionParam, this->indexOfRefraction);
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Set the scene texture
	if (this->sceneTex != NULL) {
		cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
		cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
		cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());
	}

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->noiseScaleParam, this->scale);
	cgGLSetParameter1f(this->noiseFreqParam, this->freq);
	cgGLSetParameter3f(this->noiseFlowDirParam, this->flowDir[0], this->flowDir[1], this->flowDir[2]);
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);
}