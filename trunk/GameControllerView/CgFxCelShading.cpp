#include "CgFxCelShading.h"
#include "CgShaderManager.h"
#include "GameAssets.h"
#include "Camera.h"

#include "../Utils/Matrix.h"

const std::string CgFxCelShading::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxCelShading::TEXTURED_TECHNIQUE_NAME		= "Textured";

CGeffect CgFxCelShading::cgCelShadingEffect = NULL;
std::map<std::string, CGtechnique> CgFxCelShading::cgCelShadingTechniques;
Texture1D* CgFxCelShading::celDiffuseTexture = NULL;

bool CgFxCelShading::isCelShadingEffectLoaded = false;

// Cg parameter pointers

// Transforms
CGparameter CgFxCelShading::worldITMatrixParam	= NULL;
CGparameter CgFxCelShading::wvpMatrixParam			= NULL;
CGparameter CgFxCelShading::worldMatrixParam		= NULL;
CGparameter CgFxCelShading::viewInvMatrixParam	= NULL;

// Textures
CGparameter CgFxCelShading::texSamplerParam		= NULL;
CGparameter CgFxCelShading::celSamplerParam		= NULL;

// Material properties
CGparameter CgFxCelShading::diffuseColourParam	= NULL;
CGparameter CgFxCelShading::shininessParam			= NULL;
CGparameter CgFxCelShading::specularColourParam = NULL;

// Lights
CGparameter CgFxCelShading::diffuseLightPosParam		= NULL;
CGparameter CgFxCelShading::diffuseLightColourParam	= NULL;


// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading(MaterialProperties* properties) : CgFxEffect(properties) {
	
	this->Load();

	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = cgCelShadingTechniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = cgCelShadingTechniques[BASIC_TECHNIQUE_NAME];
	}

}

CgFxCelShading::~CgFxCelShading() {
	delete this->properties;
	this->Unload();
}

/**
 * Assign the parameters required for the cel shader.
 */
void CgFxCelShading::SetupBeforePasses(const Camera& camera) {
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
	cgGLSetTextureParameter(celSamplerParam, this->celDiffuseTexture->GetTextureID());
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
 * Function called by CgShaderManager to load this effect.
 */
void CgFxCelShading::Load() {
	if (isCelShadingEffectLoaded) {
		return;
	}
	
	// Load the celshading effect from file
	cgCelShadingEffect = CgShaderManager::Instance()->LoadEffectFromCgFxFile("resources/shaders/CelShading.cgfx");

	// Load the parameters for celshading
	// Transforms
	worldITMatrixParam	= cgGetNamedEffectParameter(cgCelShadingEffect, "WorldITXf");
	wvpMatrixParam			= cgGetNamedEffectParameter(cgCelShadingEffect, "WvpXf");
	worldMatrixParam		= cgGetNamedEffectParameter(cgCelShadingEffect, "WorldXf");
	viewInvMatrixParam	= cgGetNamedEffectParameter(cgCelShadingEffect, "ViewIXf");

	// Textures
	texSamplerParam	= cgGetNamedEffectParameter(cgCelShadingEffect, "ColorSampler");
	celSamplerParam	= cgGetNamedEffectParameter(cgCelShadingEffect, "CelSampler");

	// Material properties
	diffuseColourParam		= cgGetNamedEffectParameter(cgCelShadingEffect, "DiffuseColour");
	specularColourParam		= cgGetNamedEffectParameter(cgCelShadingEffect, "SpecularColour");
	shininessParam				= cgGetNamedEffectParameter(cgCelShadingEffect, "Shininess");

	// Lights
	diffuseLightPosParam			= cgGetNamedEffectParameter(cgCelShadingEffect, "SceneLightPos");
	diffuseLightColourParam		= cgGetNamedEffectParameter(cgCelShadingEffect, "SceneLightColor");


	CgShaderManager::Instance()->CheckForCgError("Binding parameters for CgFxCelShading");

	// Load the techniques for celshading
	cgCelShadingTechniques = CgShaderManager::Instance()->LoadTechniques(cgCelShadingEffect);

	celDiffuseTexture = Texture1D::CreateTexture1DFromImgFile("resources/textures/celshading_texture1x256.jpg", Texture::Nearest, false);
	isCelShadingEffectLoaded = true;
}

/**
 * Function called by CgShaderManager to unload this effect.
 */
void CgFxCelShading::Unload() {
	if (!isCelShadingEffectLoaded) { return; }

	delete celDiffuseTexture;
	isCelShadingEffectLoaded = false;
}