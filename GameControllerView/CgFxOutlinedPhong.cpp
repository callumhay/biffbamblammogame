#include "CgFxOutlinedPhong.h"
#include "CgShaderManager.h"
#include "GameAssets.h"

const std::string CgFxOutlinedPhong::EFFECT_FILE							= "resources/shaders/OutlinedPhong.cgfx";
const std::string CgFxOutlinedPhong::BASIC_TECHNIQUE_NAME			= "Basic";
const std::string CgFxOutlinedPhong::TEXTURED_TECHNIQUE_NAME	= "Textured";

// Default constructor, builds default, white material
CgFxOutlinedPhong::CgFxOutlinedPhong(MaterialProperties* properties) : CgFxEffect(EFFECT_FILE, properties) {

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