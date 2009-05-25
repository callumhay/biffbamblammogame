#include "CgFxPhong.h"
#include "GameViewConstants.h"

const std::string CgFxPhong::BASIC_FG_TECHNIQUE_NAME			= "BasicFG";
const std::string CgFxPhong::TEXTURED_FG_TECHNIQUE_NAME		= "TexturedFG";
const std::string CgFxPhong::BASIC_BG_TECHNIQUE_NAME			= "BasicBG";
const std::string CgFxPhong::TEXTURED_BG_TECHNIQUE_NAME		= "TexturedBG";

// Default constructor, builds default, white material
CgFxPhong::CgFxPhong(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PHONG_SHADER, properties) {

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

	debug_cg_state();
}

CgFxPhong::~CgFxPhong() {
}

void CgFxPhong::SetupBeforePasses(const Camera& camera) {
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
	CgFxMaterialEffect::SetupBeforePasses(camera);
}