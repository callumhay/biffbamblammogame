#include "CgFxCelShading.h"
#include "CgShaderManager.h"
#include "GameAssets.h"

#include "../Utils/Matrix.h"

const std::string CgFxCelShading::EFFECT_FILE								= "resources/shaders/CelShading.cgfx";
const std::string CgFxCelShading::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxCelShading::TEXTURED_TECHNIQUE_NAME		= "Textured";

// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading(MaterialProperties* properties) : CgFxEffect(EFFECT_FILE, properties) {

	// Set up the cel shading texture parameter, unique to this material
	this->celSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "CelSampler");
	this->celDiffuseTexture = Texture1D::CreateTexture1DFromImgFile("resources/textures/celshading_texture1x256.jpg", Texture::Nearest, false);
	cgGLSetTextureParameter(this->celSamplerParam, this->celDiffuseTexture->GetTextureID());

	// If there's a diffuse texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];
	}
}

CgFxCelShading::~CgFxCelShading() {
	delete celDiffuseTexture;
}