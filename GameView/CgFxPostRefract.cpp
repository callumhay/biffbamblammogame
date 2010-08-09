#include "CgFxPostRefract.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxPostRefract::BASIC_TECHNIQUE_NAME						= "PostRefractGeom";
const char* CgFxPostRefract::NORMAL_TEXTURE_TECHNIQUE_NAME	= "PostRefractNormalTex";

// Default constructor
CgFxPostRefract::CgFxPostRefract() : 
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_POSTREFRACT_SHADER), 
indexOfRefraction(1.00f), warpAmount(1.0f), sceneTex(NULL), normalTex(NULL) {
	
	// Set the technique
	this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];

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

	// The rendered scene background texture
	this->sceneSamplerParam  = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->normalSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NormalSampler");
	
	debug_cg_state();
}

CgFxPostRefract::~CgFxPostRefract() {
}

void CgFxPostRefract::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam,     CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam,   CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

	const Matrix4x4& invViewXf = camera.GetInvViewTransform();
	cgGLSetMatrixParameterfc(this->viewInvMatrixParam, invViewXf.begin());

	// Refraction property setup
	cgGLSetParameter1f(this->indexOfRefactionParam, this->indexOfRefraction);
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Set the scene texture
	if (this->sceneTex != NULL) {
		cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
		cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
		cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());
	}
	// If there's a normal texture, set it
	if (this->normalTex != NULL) {
		cgGLSetTextureParameter(this->normalSamplerParam, this->normalTex->GetTextureID());
	}
}