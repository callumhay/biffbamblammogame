#include "CgFxPostRefract.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgShaderManager.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const std::string CgFxPostRefract::BASIC_TECHNIQUE_NAME			= "PostRefractGeom";
const std::string CgFxPostRefract::VAPOUR_TRAIL_TECHNIQUE_NAME = "VapourTrail";

// Default constructor
CgFxPostRefract::CgFxPostRefract() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_POSTREFRACT_SHADER), 
invisiColour(Colour(1,1,1)), indexOfRefraction(1.00f), scale(1.0f), freq(1.0f), flowDir(0, 0, 1),
timer(0.0), warpAmount(1.0f), sceneTex(NULL), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {
	
	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

	// Transform parameters
	this->worldITMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	this->viewInvMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

	// Refraction parameters
	this->indexOfRefactionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->invisiColourParam			= cgGetNamedEffectParameter(this->cgEffect, "InvisiColour");
	this->warpAmountParam			  = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	// Vapour trail parameters
	this->noiseScaleParam				= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->noiseFreqParam				= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->noiseFlowDirParam			= cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->timerParam						= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->noiseSamplerParam			= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->maskSamplerParam			= cgGetNamedEffectParameter(this->cgEffect, "MaskSampler");

	// The rendered scene background texture
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	CgShaderManager::Instance()->CheckForCgError("Getting parameters for CgFxPostRefract");
}

CgFxPostRefract::~CgFxPostRefract() {
}

void CgFxPostRefract::SetupBeforePasses(const Camera& camera) {
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
	cgGLSetParameter3f(this->invisiColourParam, this->invisiColour.R(), this->invisiColour.G(), this->invisiColour.B());
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Set the scene texture
	if (this->sceneTex != NULL) {
		cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
		cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
		cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());
	}

	// Noise property setup
	if (this->currTechnique == this->techniques[VAPOUR_TRAIL_TECHNIQUE_NAME]) {
		cgGLSetParameter1f(this->timerParam, this->timer);
		cgGLSetParameter1f(this->noiseScaleParam, this->scale);
		cgGLSetParameter1f(this->noiseFreqParam, this->freq);
		cgGLSetParameter3f(this->noiseFlowDirParam, this->flowDir[0], this->flowDir[1], this->flowDir[2]);
		cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);

		// Set the mask texture sampler if it exists
		if (this->maskTex != NULL) {
			cgGLSetTextureParameter(this->maskSamplerParam, this->maskTex->GetTextureID());
		}
	}

}