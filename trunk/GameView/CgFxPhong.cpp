/**
 * CgFxPhong.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxPhong.h"
#include "GameViewConstants.h"

const char* CgFxPhong::BASIC_FG_TECHNIQUE_NAME                  = "BasicFG";
const char* CgFxPhong::BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME       = "BasicFGNoOutline";
const char* CgFxPhong::TEXTURED_FG_TECHNIQUE_NAME               = "TexturedFG";
const char* CgFxPhong::TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME	= "TexturedFGNoOutline";
const char* CgFxPhong::BASIC_BG_TECHNIQUE_NAME                  = "BasicBG";
const char* CgFxPhong::TEXTURED_BG_TECHNIQUE_NAME               = "TexturedBG";

// Default constructor, builds default, white material
CgFxPhong::CgFxPhong(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PHONG_SHADER, properties) {
	assert(properties->materialType == MaterialProperties::MATERIAL_PHONG_TYPE);

	// Set the correct technique based on whether there's a texture and whether the
	// geometry is in the foreground or background and whether there is an outline or not
	
    if (this->properties->diffuseTexture != NULL) {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {

			if (this->properties->outlineSize < EPSILON) {
				this->currTechnique = this->techniques[TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME];
			}
			else {
				this->currTechnique = this->techniques[TEXTURED_FG_TECHNIQUE_NAME];
			}
		}
		else {
			this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
		}
	}
	else {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			if (this->properties->outlineSize < EPSILON) {
				this->currTechnique = this->techniques[BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME];
			}
			else {
				this->currTechnique = this->techniques[BASIC_FG_TECHNIQUE_NAME];
			}
		}
		else {
			this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
		}
	}

	debug_cg_state();
}

CgFxPhong::~CgFxPhong() {
}
