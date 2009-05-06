#include "CgFxCelShading.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgShaderManager.h"
#include "../BlammoEngine/Matrix.h"

const std::string CgFxCelShading::BASIC_FG_TECHNIQUE_NAME			= "BasicFG";
const std::string CgFxCelShading::TEXTURED_FG_TECHNIQUE_NAME	= "TexturedFG";
const std::string CgFxCelShading::BASIC_BG_TECHNIQUE_NAME			= "BasicBG";
const std::string CgFxCelShading::TEXTURED_BG_TECHNIQUE_NAME	= "TexturedBG";
Texture1D* CgFxCelShading::CelDiffuseTexture = NULL;

// Default constructor, builds default, white material
CgFxCelShading::CgFxCelShading(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_CEL_SHADER, properties) {

	// Set up the cel-shading texture parameter, unique to this material
	this->celSamplerParam = NULL;
	this->celSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "CelShadingSampler");
	assert(this->celSamplerParam);

	// Initialize the static cel texture if it hasn't been already
	if (this->CelDiffuseTexture  == NULL) {
		this->CelDiffuseTexture = Texture1D::CreateTexture1DFromImgFile("resources/textures/celshading_texture1x256.jpg", Texture::Nearest);
		assert(this->CelDiffuseTexture != NULL);
	}
	cgGLSetTextureParameter(this->celSamplerParam, this->CelDiffuseTexture->GetTextureID());

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
	if (this->CelDiffuseTexture != NULL) {
		delete this->CelDiffuseTexture;
		this->CelDiffuseTexture = NULL;
	}
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
	
	cgGLSetParameter1f(this->outlineWidthParam, this->properties->outlineSize);
	cgGLSetParameter3f(this->outlineColourParam, this->properties->outlineColour.R(),this->properties->outlineColour.G(), this->properties->outlineColour.B());

	CgFxMaterialEffect::SetupBeforePasses(camera);
}