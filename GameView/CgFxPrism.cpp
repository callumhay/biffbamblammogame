/**
 * CgFxPrism.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxPrism.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

const char* CgFxPrism::DEFAULT_PRISM_TECHNIQUE_NAME = "Prism";

CgFxPrism::CgFxPrism(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PRISM_SHADER, properties),
indexOfRefractionParam(NULL), warpAmountParam(NULL), sceneWidthParam(NULL), sceneHeightParam(NULL), 
sceneSamplerParam(NULL), indexOfRefraction(1.6), warpAmount(200) {

	assert(properties->materialType == MaterialProperties::MATERIAL_PRISM_TYPE);

	this->currTechnique = this->techniques[DEFAULT_PRISM_TECHNIQUE_NAME];

	this->indexOfRefractionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam        = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam        = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");
	this->sceneSamplerParam      = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	// Set the appropriate diffuse colour
	this->properties->diffuse   = GameViewConstants::GetInstance()->PRISM_BLOCK_COLOUR;
	this->properties->specular  = Colour(0.7f, 0.7f, 0.7f);
    this->properties->shininess = 90.0f;
    this->properties->geomType  = MaterialProperties::MATERIAL_GEOM_FG_TYPE;

	debug_cg_state();
}

CgFxPrism::~CgFxPrism() {
}

void CgFxPrism::SetupBeforePasses(const Camera& camera) {
	cgGLSetParameter1f(this->sceneWidthParam, camera.GetWindowWidth());
	cgGLSetParameter1f(this->sceneHeightParam, camera.GetWindowHeight());
	cgGLSetParameter1f(this->warpAmountParam, this->warpAmount);
	cgGLSetParameter1f(this->indexOfRefractionParam, this->indexOfRefraction);
	
	assert(this->sceneTex != NULL);
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

	CgFxMaterialEffect::SetupBeforePasses(camera);
}