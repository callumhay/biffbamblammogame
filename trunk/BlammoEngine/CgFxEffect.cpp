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

const std::string MaterialProperties::MATERIAL_CELBASIC_TYPE	= "outlinedcel";
const std::string MaterialProperties::MATERIAL_PHONG_TYPE			= "phong";
const std::string MaterialProperties::MATERIAL_INKBLOCK_TYPE	= "inkblock";
const std::string MaterialProperties::MATERIAL_STICKYGOO_TYPE	= "stickygoo";

const std::string MaterialProperties::MATERIAL_GEOM_FG_TYPE = "fg";
const std::string MaterialProperties::MATERIAL_GEOM_BG_TYPE = "bg";

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
				 this->ballPointLightAttenParam);
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
	
	// Lights - depending on whether this is a foreground or background effect - no matter what there
	// is always a key and a fill light
	Point3D keyLightPos			= this->keyLight.GetPosition();
	Colour  keyLightCol			= this->keyLight.GetDiffuseColour();
	Point3D fillLightPos		= this->fillLight.GetPosition();
	Colour  fillLightCol		= this->fillLight.GetDiffuseColour();
	float fillLightLinearAtten	= this->fillLight.GetLinearAttenuation();

	// Key
	cgGLSetParameter3f(this->keyPointLightPosParam,    keyLightPos[0], keyLightPos[1], keyLightPos[2]);
	cgGLSetParameter3f(this->keyPointLightColourParam, keyLightCol[0], keyLightCol[1], keyLightCol[2]);

	// Fill
	cgGLSetParameter3f(this->fillPointLightPosParam,    fillLightPos[0], fillLightPos[1], fillLightPos[2]);
	cgGLSetParameter3f(this->fillPointLightColourParam, fillLightCol[0], fillLightCol[1], fillLightCol[2]);
	cgGLSetParameter1f(this->fillPointLightAttenParam,  fillLightLinearAtten);

	if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
		// Foreground: also has a Ball light
		
		Point3D ballLightPos				= this->ballLight.GetPosition();
		Colour ballLightDiffuse			= this->ballLight.GetDiffuseColour();
		float ballLightLinearAtten	= this->ballLight.GetLinearAttenuation();

		// Ball
		cgGLSetParameter3f(this->ballPointLightPosParam,     ballLightPos[0], ballLightPos[1], ballLightPos[2]);
		cgGLSetParameter3f(this->ballPointLightDiffuseParam, ballLightDiffuse[0], ballLightDiffuse[1], ballLightDiffuse[2]);
		cgGLSetParameter1f(this->ballPointLightAttenParam,   ballLightLinearAtten);	
	}

	debug_cg_state();
}