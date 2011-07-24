/**
 * CgFxEffect.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxEffect.h"
#include "Camera.h"
#include "Matrix.h"

#include "../ResourceManager.h"

CgFxEffectBase::CgFxEffectBase(const std::string& effectPath) : cgEffect(NULL), currTechnique(NULL) {
	// Load the effect and its techniques from file
	ResourceManager::GetInstance()->GetCgFxEffectResource(effectPath, this->cgEffect, this->techniques);
	assert(this->cgEffect != NULL);
	assert(this->techniques.size() > 0);
}

CgFxEffectBase::~CgFxEffectBase() {
	// Release the effect resource
	bool success = ResourceManager::GetInstance()->ReleaseCgFxEffectResource(this->cgEffect);
	assert(success);
}

const char* MaterialProperties::MATERIAL_CELBASIC_TYPE	= "outlinedcel";
const char* MaterialProperties::MATERIAL_PHONG_TYPE			= "phong";
const char* MaterialProperties::MATERIAL_INKBLOCK_TYPE	= "inkblock";
const char* MaterialProperties::MATERIAL_STICKYGOO_TYPE	= "stickygoo";
const char* MaterialProperties::MATERIAL_PRISM_TYPE			= "prism";
const char* MaterialProperties::MATERIAL_PORTAL_TYPE		= "portal";

const char* MaterialProperties::MATERIAL_GEOM_FG_TYPE = "fg";
const char* MaterialProperties::MATERIAL_GEOM_BG_TYPE = "bg";

CgFxMaterialEffect::CgFxMaterialEffect(const std::string& effectPath, MaterialProperties* props) : 
CgFxEffectBase(effectPath), properties(props),

keyLight(Point3D(-25.0f, 20.0f, 50.0f), Colour(0.932f, 1.0f, 0.755f), 0.0f),
fillLight(Point3D(30.0f, 30.0f, 50.0f), Colour(1.0f, 0.434f, 0.92f), 0.03f),
ballLight(Point3D(0,0,0), Colour(1,1,1), 0.0f)
{
	assert(props != NULL);
	this->LoadParameters();
}

CgFxMaterialEffect::~CgFxMaterialEffect() {
	delete this->properties;
	this->properties = NULL;
}

/**
 * Private helper for loading the parameters of the currently loaded effect.
 */
void CgFxMaterialEffect::LoadParameters() {
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
	this->alphaMultiplierParam	= NULL;
	
	this->keyPointLightPosParam				= NULL;
	this->keyPointLightColourParam		= NULL;
	this->fillPointLightPosParam			= NULL;
	this->fillPointLightColourParam		= NULL;
	this->fillPointLightAttenParam		= NULL;
	this->ballPointLightPosParam			= NULL;
	this->ballPointLightDiffuseParam	= NULL;
	this->ballPointLightAttenParam		= NULL;

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
	this->alphaMultiplierParam  = cgGetNamedEffectParameter(this->cgEffect, "AlphaMultiplier");

	// Lights: Key light, Fill light and Ball light
	this->keyPointLightPosParam			= cgGetNamedEffectParameter(this->cgEffect, "KeyPointLightPos");
	this->keyPointLightColourParam	= cgGetNamedEffectParameter(this->cgEffect, "KeyPointLightColour");
	this->fillPointLightPosParam		= cgGetNamedEffectParameter(this->cgEffect, "FillPointLightPos");
	this->fillPointLightColourParam	= cgGetNamedEffectParameter(this->cgEffect, "FillPointLightColour");
	this->fillPointLightAttenParam	= cgGetNamedEffectParameter(this->cgEffect, "FillPointLightLinearAtten");
	this->ballPointLightPosParam		= cgGetNamedEffectParameter(this->cgEffect, "BallPointLightPos");
	this->ballPointLightDiffuseParam	= cgGetNamedEffectParameter(this->cgEffect, "BallPointLightDiffuse");
	this->ballPointLightAttenParam	= cgGetNamedEffectParameter(this->cgEffect, "BallPointLightLinearAtten");

	debug_cg_state();
	assert(this->worldITMatrixParam	&& this->wvpMatrixParam	&& this->worldMatrixParam	&&
				 this->viewInvMatrixParam	&& this->texSamplerParam && this->diffuseColourParam &&
				 this->shininessParam	&& this->specularColourParam && this->keyPointLightPosParam && 
				 this->keyPointLightColourParam && this->fillPointLightPosParam && this->fillPointLightColourParam &&
				 this->fillPointLightAttenParam && this->ballPointLightPosParam && this->ballPointLightDiffuseParam &&
				 this->ballPointLightAttenParam && this->alphaMultiplierParam);
}

/**
 * Private helper function for setting up the parameters for the effect.
 */
void CgFxMaterialEffect::SetupBeforePasses(const Camera& camera) {
	// Transforms
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	// Obtain the current model view and inverse view transforms
	float tempMVXfVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	Matrix4x4 tempMVXf(tempMVXfVals);
	const Matrix4x4& invViewXf = camera.GetInvViewTransform();
    cgGLSetMatrixParameterfc(this->viewInvMatrixParam, invViewXf.begin());

	// Make sure that JUST the world transform is set
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invViewXf.begin());
	glMultMatrixf(tempMVXf.begin());
	// Set the world transform parameters
	cgGLSetStateMatrixParameter(this->worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();    
    
	// Textures
	if (this->properties->diffuseTexture != NULL) {
		cgGLSetTextureParameter(this->texSamplerParam, this->properties->diffuseTexture->GetTextureID());
		cgSetSamplerState(this->texSamplerParam);
	}

	// Material properties
	cgGLSetParameter3fv(this->diffuseColourParam, this->properties->diffuse.begin());
	cgGLSetParameter3fv(this->specularColourParam, this->properties->specular.begin());
	cgGLSetParameter1f(this->shininessParam, this->properties->shininess);
	cgGLSetParameter1f(this->alphaMultiplierParam, this->properties->alphaMultiplier);
	
	// Lights - depending on whether this is a foreground or background effect - no matter what there
	// is always a key and a fill light
	const Point3D& keyLightPos		= this->keyLight.GetPosition();
	const Colour&  keyLightCol		= this->keyLight.GetDiffuseColour();
	const Point3D& fillLightPos		= this->fillLight.GetPosition();
	const Colour&  fillLightCol		= this->fillLight.GetDiffuseColour();
	float fillLightLinearAtten		= this->fillLight.GetLinearAttenuation();

	// Key
	cgGLSetParameter3fv(this->keyPointLightPosParam, keyLightPos.begin());
	cgGLSetParameter3fv(this->keyPointLightColourParam, keyLightCol.begin());

	// Fill
	cgGLSetParameter3fv(this->fillPointLightPosParam,    fillLightPos.begin());
	cgGLSetParameter3fv(this->fillPointLightColourParam, fillLightCol.begin());
	cgGLSetParameter1f(this->fillPointLightAttenParam,   fillLightLinearAtten);

	if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
		// Foreground: also has a Ball light
		
		const Point3D& ballLightPos      = this->ballLight.GetPosition();
		const Colour& ballLightDiffuse   = this->ballLight.GetDiffuseColour();
		float ballLightLinearAtten       = this->ballLight.GetLinearAttenuation();

		// Ball
		cgGLSetParameter3fv(this->ballPointLightPosParam,     ballLightPos.begin());
		cgGLSetParameter3fv(this->ballPointLightDiffuseParam, ballLightDiffuse.begin());
		cgGLSetParameter1f(this->ballPointLightAttenParam,    ballLightLinearAtten);	
	}

	debug_cg_state();
}