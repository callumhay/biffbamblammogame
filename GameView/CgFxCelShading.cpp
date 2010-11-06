#include "CgFxCelShading.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Matrix.h"
#include "../ResourceManager.h"

const char* CgFxCelShading::BASIC_FG_TECHNIQUE_NAME			= "BasicFG";
const char* CgFxCelShading::TEXTURED_FG_TECHNIQUE_NAME	= "TexturedFG";
const char* CgFxCelShading::BASIC_BG_TECHNIQUE_NAME			= "BasicBG";
const char* CgFxCelShading::TEXTURED_BG_TECHNIQUE_NAME	= "TexturedBG";
Texture* CgFxCelShading::CelDiffuseTexture = NULL;

// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_CEL_SHADER, properties) {
	assert(properties->materialType == MaterialProperties::MATERIAL_CELBASIC_TYPE);

	// Set up the cel-shading texture parameter, unique to this material
	this->celSamplerParam = NULL;
	this->celSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "CelShadingSampler");
	assert(this->celSamplerParam);

	// Initialize the static cel gradient texture
	CgFxCelShading::CelDiffuseTexture = ResourceManager::GetInstance()->GetCelShadingTexture();
	assert(CgFxCelShading::CelDiffuseTexture != NULL);

	// Set up other cel-shading parameters
	this->outlineWidthParam		= NULL;
	this->outlineColourParam	= NULL;
	this->outlineWidthParam		=	cgGetNamedEffectParameter(this->cgEffect, "OutlineWidth");
	this->outlineColourParam	=	cgGetNamedEffectParameter(this->cgEffect, "OutlineColour");
	assert(this->outlineWidthParam && this->outlineColourParam);

	// Set the correct technique based on whether there's a texture and whether the
	// geometry is in the foreground or background
	if (this->properties->diffuseTexture != NULL) {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			this->currTechnique = this->techniques[TEXTURED_FG_TECHNIQUE_NAME];
		}
		else {
			this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
		}
	}
	else {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			this->currTechnique = this->techniques[BASIC_FG_TECHNIQUE_NAME];
		}
		else {
			this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
		}
	}

}

CgFxCelShading::~CgFxCelShading() {
}

void CgFxCelShading::SetupBeforePasses(const Camera& camera) {
	// Set the correct technique based on whether there's a texture and whether the
	// geometry is in the foreground or background
	if (this->properties->diffuseTexture != NULL) {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			this->currTechnique = this->techniques[TEXTURED_FG_TECHNIQUE_NAME];
		}
		else {
			this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
		}
	}
	else {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			this->currTechnique = this->techniques[BASIC_FG_TECHNIQUE_NAME];
		}
		else {
			this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
		}
	}

	cgGLSetTextureParameter(this->celSamplerParam, CgFxCelShading::CelDiffuseTexture->GetTextureID());
	cgGLSetParameter1f(this->outlineWidthParam, this->properties->outlineSize);
	cgGLSetParameter3fv(this->outlineColourParam, this->properties->outlineColour.begin());

	CgFxMaterialEffect::SetupBeforePasses(camera);
}