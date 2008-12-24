#include "CgFxCelShading.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgShaderManager.h"
#include "../BlammoEngine/Matrix.h"

const std::string CgFxCelShading::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxCelShading::TEXTURED_TECHNIQUE_NAME		= "Textured";

// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_CEL_SHADER, properties) {

	// Set up the cel-shading texture parameter, unique to this material
	this->celSamplerParam = NULL;
	this->celSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "CelSampler");
	assert(this->celSamplerParam);
	this->celDiffuseTexture = Texture1D::CreateTexture1DFromImgFile("resources/textures/celshading_texture1x256.jpg", Texture::Nearest);
	cgGLSetTextureParameter(this->celSamplerParam, this->celDiffuseTexture->GetTextureID());

	// Set up other cel-shading parameters
	this->outlineWidthParam		= NULL;
	this->outlineColourParam	= NULL;
	this->outlineWidthParam		=	cgGetNamedEffectParameter(this->cgEffect, "OutlineWidth");
	this->outlineColourParam	=	cgGetNamedEffectParameter(this->cgEffect, "OutlineColour");
	assert(this->outlineWidthParam && this->outlineColourParam);

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

void CgFxCelShading::SetupBeforePasses(const Camera& camera) {
	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];
	}
	
	cgGLSetParameter1f(this->outlineWidthParam, this->properties->outlineSize);
	cgGLSetParameter3f(this->outlineColourParam, this->properties->outlineColour.R(),this->properties->outlineColour.G(), this->properties->outlineColour.B());

	CgFxMaterialEffect::SetupBeforePasses(camera);
}