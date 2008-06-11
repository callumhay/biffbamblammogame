#include "CgFxOutlinedPhong.h"
#include "CgShaderManager.h"
#include "GameAssets.h"
#include "Camera.h"

#include "../Utils/Matrix.h"

const std::string CgFxOutlinedPhong::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxOutlinedPhong::TEXTURED_TECHNIQUE_NAME	= "Textured";

CGeffect CgFxOutlinedPhong::cgOutlinedPhongEffect = NULL;
std::map<std::string, CGtechnique> CgFxOutlinedPhong::cgOutlinedPhongTechniques;

bool CgFxOutlinedPhong::isOutlinedPhongEffectLoaded	= false;

// Cg parameter pointers

// Transforms
CGparameter CgFxOutlinedPhong::worldITMatrixParam	= NULL;
CGparameter CgFxOutlinedPhong::wvpMatrixParam			= NULL;
CGparameter CgFxOutlinedPhong::worldMatrixParam		= NULL;
CGparameter CgFxOutlinedPhong::viewInvMatrixParam	= NULL;

// Textures
CGparameter CgFxOutlinedPhong::texSamplerParam		= NULL;

// Material properties
CGparameter CgFxOutlinedPhong::diffuseColourParam		= NULL;
CGparameter CgFxOutlinedPhong::specularColourParam	= NULL;
CGparameter CgFxOutlinedPhong::shininessParam				= NULL;

// Lights
CGparameter CgFxOutlinedPhong::diffuseLightPosParam		= NULL;
CGparameter CgFxOutlinedPhong::diffuseLightColourParam	= NULL;


// Default constructor, builds default, white material
CgFxOutlinedPhong::CgFxOutlinedPhong(MaterialProperties* properties) : CgFxEffect(properties) {

	this->Load();

	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = cgOutlinedPhongTechniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = cgOutlinedPhongTechniques[BASIC_TECHNIQUE_NAME];
	}
}

CgFxOutlinedPhong::~CgFxOutlinedPhong() {
	delete this->properties;
	this->Unload();
}

/**
 * Assign the parameters required for the cel shader.
 */
void CgFxOutlinedPhong::SetupBeforePasses(const Camera& camera) {
	// Transforms
	cgGLSetStateMatrixParameter(wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

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
	cgGLSetStateMatrixParameter(viewInvMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glMultMatrixf(tempMVXf.begin());
	// Set the world transform parameters
	cgGLSetStateMatrixParameter(worldITMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	cgGLSetStateMatrixParameter(worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();

	// Textures
	if (this->properties->diffuseTexture != NULL) {
		cgGLSetTextureParameter(texSamplerParam, this->properties->diffuseTexture->GetTextureID());
	}

	// Material properties
	cgGLSetParameter3f(diffuseColourParam, this->properties->diffuse.R(), this->properties->diffuse.G(), this->properties->diffuse.B());
	cgGLSetParameter3f(specularColourParam, this->properties->specular.R(), this->properties->specular.G(), this->properties->specular.B());
	cgGLSetParameter1f(shininessParam, this->properties->shininess);

	// Lights... TODO...
	cgGLSetParameter3f(diffuseLightPosParam, 0.0f, 20.0f, 50.0f);
	cgGLSetParameter3f(diffuseLightColourParam, 1.0f, 1.0f, 1.0f);
}

/**
 * Function called by the superclass constructor to load this effect.
 */
void CgFxOutlinedPhong::Load() {
	if (isOutlinedPhongEffectLoaded) {
		return;
	}

	// Load the outlined phong effect from file
	cgOutlinedPhongEffect = CgShaderManager::Instance()->LoadEffectFromCgFxFile("resources/shaders/OutlinedPhong.cgfx");

	// Load the parameters for outlined phong
	// Transforms
	worldITMatrixParam	= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "WorldITXf");
	wvpMatrixParam			= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "WvpXf");
	worldMatrixParam		= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "WorldXf");
	viewInvMatrixParam	= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "ViewIXf");

	// Textures
	texSamplerParam	= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "ColorSampler");

	// Material properties
	diffuseColourParam		= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "DiffuseColour");
	specularColourParam		= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "SpecularColour");
	shininessParam				= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "Shininess");
	
	// Lights
	diffuseLightPosParam			= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "SceneLightPos");
	diffuseLightColourParam		= cgGetNamedEffectParameter(cgOutlinedPhongEffect, "SceneLightColor");

	CgShaderManager::Instance()->CheckForCgError("Binding parameters for CgFxCelShading");

	// Load the techniques for outlined phong
	cgOutlinedPhongTechniques = CgShaderManager::Instance()->LoadTechniques(cgOutlinedPhongEffect);
	isOutlinedPhongEffectLoaded = true;
}

void CgFxOutlinedPhong::Unload() {
	if (!isOutlinedPhongEffectLoaded) {
		return;
	}
	// Unload here
	isOutlinedPhongEffectLoaded = false;
}