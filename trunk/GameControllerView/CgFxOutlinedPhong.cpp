#include "CgFxOutlinedPhong.h"
#include "CgShaderManager.h"
#include "GameAssets.h"

const std::string CgFxOutlinedPhong::EFFECT_FILE							= "resources/shaders/OutlinedPhong.cgfx";
const std::string CgFxOutlinedPhong::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxOutlinedPhong::TEXTURED_TECHNIQUE_NAME	= "Textured";

// Default constructor, builds default, white material
CgFxOutlinedPhong::CgFxOutlinedPhong(MaterialProperties* properties) : CgFxEffect(EFFECT_FILE, properties) {
	// Set up cel-shading parameters
	this->outlineWidthParam		= NULL;
	this->outlineColourParam	= NULL;
	this->outlineWidthParam		=	cgGetNamedEffectParameter(this->cgEffect, "OutlineWidth");
	this->outlineColourParam	=	cgGetNamedEffectParameter(this->cgEffect, "OutlineColour");
	assert(this->outlineWidthParam && this->outlineColourParam);

	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];
	}

}

CgFxOutlinedPhong::~CgFxOutlinedPhong() {
}

void CgFxOutlinedPhong::SetupBeforePasses(const Camera& camera) {
	// If there's a texture set the correct technique
	if (this->properties->diffuseTexture != NULL) {
		this->currTechnique = this->techniques[TEXTURED_TECHNIQUE_NAME];
	}
	else {
		this->currTechnique = this->techniques[BASIC_TECHNIQUE_NAME];
	}

	cgGLSetParameter1f(this->outlineWidthParam, this->properties->outlineSize);
	cgGLSetParameter3f(this->outlineColourParam, this->properties->outlineColour.R(),this->properties->outlineColour.G(), this->properties->outlineColour.B());

	CgFxEffect::SetupBeforePasses(camera);
}