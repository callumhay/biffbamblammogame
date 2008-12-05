#include "CgFxPostRefract.h"
#include "CgShaderManager.h"
#include "Camera.h"

#include "Matrix.h"

const std::string CgFxPostRefract::EFFECT_FILE							= "resources/shaders/PostRefract.cgfx";
const std::string CgFxPostRefract::BASIC_TECHNIQUE_NAME			= "PostRefractGeom";

// Default constructor, builds default, white material
CgFxPostRefract::CgFxPostRefract() : 
CgFxEffectBase(EFFECT_FILE), invisiColour(Colour(1,1,1)), indexOfRefraction(1.00f), 
warpAmount(1.0f), sceneTex(NULL) {
	
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
}