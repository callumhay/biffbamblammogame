#include "CgFxEffect.h"
#include "CgShaderManager.h"
#include "Camera.h"

#include "../Utils/Matrix.h"

const std::string MaterialProperties::MATERIAL_CELBASIC_TYPE	= "outlinedcel";
const std::string MaterialProperties::MATERIAL_CELPHONG_TYPE	= "outlinedphong";

CgFxEffect::CgFxEffect(const std::string& effectPath, MaterialProperties* props) : 
properties(props), currTechnique(NULL), cgEffect(NULL)
{
	assert(props != NULL);

	// Load the effect from file
	this->cgEffect = CgShaderManager::Instance()->LoadEffectFromCgFxFile(effectPath);
	// Load the parameters
	this->LoadParameters();
	// Load the techniques
	CgShaderManager::Instance()->LoadEffectTechniques(this->cgEffect, this->techniques);
}

CgFxEffect::~CgFxEffect() {
	cgDestroyEffect(this->cgEffect);
	CgShaderManager::Instance()->CheckForCgError("Destroying effect");
	delete this->properties;
}

/**
 * Private helper for loading the parameters of the currently loaded effect.
 */
void CgFxEffect::LoadParameters() {
	assert(this->cgEffect != NULL);

	// Set all parameters initially to NULL
	this->worldITMatrixParam		= NULL;
	this->wvpMatrixParam				= NULL;
	this->worldMatrixParam			= NULL;
	this->viewInvMatrixParam		= NULL;
	this->texSamplerParam				= NULL;
	this->diffuseColourParam		= NULL;
	this->shininessParam				= NULL;
	this->specularColourParam		= NULL;
	this->sceneLightPosParam		= NULL;
	this->sceneLightColourParam = NULL;

	// Transforms
	this->worldITMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "WorldITXf");
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "WvpXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");
	this->viewInvMatrixParam	= cgGetNamedEffectParameter(this->cgEffect, "ViewIXf");

	// Textures
	this->texSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "ColourSampler");

	// Material properties
	this->diffuseColourParam		= cgGetNamedEffectParameter(this->cgEffect, "DiffuseColour");
	this->specularColourParam		= cgGetNamedEffectParameter(this->cgEffect, "SpecularColour");
	this->shininessParam				= cgGetNamedEffectParameter(this->cgEffect, "Shininess");

	// Lights
	this->sceneLightPosParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneLightPos");
	this->sceneLightColourParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneLightColour");

	CgShaderManager::Instance()->CheckForCgError("Getting parameters for CgFxEffect");
	assert(this->worldITMatrixParam	&& this->wvpMatrixParam	&& this->worldMatrixParam	&&
				 this->viewInvMatrixParam	&& this->texSamplerParam && this->diffuseColourParam &&
				 this->shininessParam	&& this->specularColourParam && this->sceneLightPosParam && this->sceneLightColourParam);
}

/**
 * Private helper function for setting up the parameters for the effect.
 */
void CgFxEffect::SetupBeforePasses(const Camera& camera) {
	// Transforms
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

	// Textures
	if (this->properties->diffuseTexture != NULL) {
		cgGLSetTextureParameter(this->texSamplerParam, this->properties->diffuseTexture->GetTextureID());
	}

	// Material properties
	cgGLSetParameter3f(this->diffuseColourParam, this->properties->diffuse.R(), this->properties->diffuse.G(), this->properties->diffuse.B());
	cgGLSetParameter3f(this->specularColourParam, this->properties->specular.R(), this->properties->specular.G(), this->properties->specular.B());
	cgGLSetParameter1f(this->shininessParam, this->properties->shininess);
	// Lights... TODO...
	cgGLSetParameter3f(this->sceneLightPosParam, 0.0f, 20.0f, 50.0f);
	cgGLSetParameter3f(this->sceneLightColourParam, 1.0f, 1.0f, 1.0f);

	CgShaderManager::Instance()->CheckForCgError("Setting up CgFxEffect parameters");
}