#include "CgFxOutlinedPhong.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgShaderManager.h"

const std::string CgFxOutlinedPhong::BASIC_BG_TECHNIQUE_NAME			= "BasicBG";
const std::string CgFxOutlinedPhong::TEXTURED_BG_TECHNIQUE_NAME	= "TexturedBG";

// Default constructor, builds default, white material
CgFxOutlinedPhong::CgFxOutlinedPhong(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PHONG_SHADER, properties) {
	// Set up cel-shading parameters
	this->outlineWidthParam		= NULL;
	this->outlineColourParam	= NULL;
	this->outlineWidthParam		=	cgGetNamedEffectParameter(this->cgEffect, "OutlineWidth");
	this->outlineColourParam	=	cgGetNamedEffectParameter(this->cgEffect, "OutlineColour");
	assert(this->outlineWidthParam && this->outlineColourParam);

	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
	}

}

CgFxOutlinedPhong::~CgFxOutlinedPhong() {
}

void CgFxOutlinedPhong::SetupBeforePasses(const Camera& camera) {
	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
	}

	cgGLSetParameter1f(this->outlineWidthParam, this->properties->outlineSize);
	cgGLSetParameter3f(this->outlineColourParam, this->properties->outlineColour.R(),this->properties->outlineColour.G(), this->properties->outlineColour.B());

	CgFxMaterialEffect::SetupBeforePasses(camera);
}