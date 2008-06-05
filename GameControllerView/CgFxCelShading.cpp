#include "CgFxCelShading.h"
#include "CgShaderManager.h"
#include "GameAssets.h"

const std::string CgFxCelShading::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxCelShading::TEXTURED_TECHNIQUE_NAME		= "Textured";

CGeffect CgFxCelShading::cgCelShadingEffect = NULL;
std::map<std::string, CGtechnique> CgFxCelShading::cgCelShadingTechniques;
CGparameter CgFxCelShading::mvpMatrixParam	= NULL;
CGparameter CgFxCelShading::texSamplerParam	= NULL;


// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading() : CgFxEffect(cgCelShadingTechniques["SimpleTexture"]),
diffuse(Colour(1,1,1)), specular(Colour(0.33f, 0.33f, 0.33f)), shininess(100.0f), texture(NULL) {
}

CgFxCelShading::~CgFxCelShading() {
	if (this->texture != NULL) {
		delete this->texture;
	}
}

/**
 * Assign the parameters required for the cel shader.
 */
void CgFxCelShading::SetupBeforePasses() {
	cgGLSetStateMatrixParameter(mvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	//cgGLSetTextureParameter(texSamplerParam, this->texture->GetTextureID());
}

/**
 * Static class called by the CgShaderManager to load this effect.
 */
void CgFxCelShading::Load() {
	// Load the celshading effect from file
	cgCelShadingEffect = CgShaderManager::Instance()->LoadEffectFromCgFxFile("resources/shaders/SimpleTexture.cgfx");

	// Load the parameters for celshading
	mvpMatrixParam	= cgGetNamedEffectParameter(cgCelShadingEffect, "ModelViewProjXf");
	texSamplerParam = cgGetNamedEffectParameter(cgCelShadingEffect, "TexSampler");
	CgShaderManager::Instance()->CheckForCgError("Binding parameters for CgFxCelShading");

	// Load the techniques for celshading
	cgCelShadingTechniques = CgShaderManager::Instance()->LoadTechniques(cgCelShadingEffect);
}

void CgFxCelShading::DrawMaterial() {
	if (this->texture == NULL) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		this->texture->BindTexture();
	}
}