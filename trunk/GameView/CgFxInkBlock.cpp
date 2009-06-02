#include "CgFxInkBlock.h"
#include "GameViewConstants.h"
#include "GameDisplay.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"

const std::string CgFxInkBlock::INKBLOCK_TECHNIQUE_NAME = "InkBlock";

CgFxInkBlock::CgFxInkBlock(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_INKBLOCK_SHADER, properties),
timerParam(NULL), displacementParam(NULL), sharpnessParam(NULL), speedParam(NULL), turbDensityParam(NULL), 
timer(0.0f), displacement(1.8f), sharpness(1.5f), speed(0.35f), turbulanceDensity(1.2f) {
	
	assert(properties->materialType == MaterialProperties::MATERIAL_INKBLOCK_TYPE);
	assert(properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE);

	// Set the technique to the only one available for this effect
	this->currTechnique = this->techniques[CgFxInkBlock::INKBLOCK_TECHNIQUE_NAME];
	assert(this->currTechnique != NULL);

	// Initialize any other parameters specific to this shader effect
	this->timerParam				= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->displacementParam	= cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->sharpnessParam		= cgGetNamedEffectParameter(this->cgEffect, "Sharpness");
	this->speedParam				= cgGetNamedEffectParameter(this->cgEffect, "Speed");
	this->turbDensityParam	= cgGetNamedEffectParameter(this->cgEffect, "TurbDensity");

	debug_cg_state();
}

CgFxInkBlock::~CgFxInkBlock() {
}

/**
 * Setup all shader specific parameters and samplers before any
 * techniques / passes are executed.
 */
void CgFxInkBlock::SetupBeforePasses(const Camera& camera) {
	CgFxMaterialEffect::SetupBeforePasses(camera);

	this->timer += 0.017;	// Approx 60 fps

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->sharpnessParam, this->sharpness);
	cgGLSetParameter1f(this->speedParam, this->speed);
	cgGLSetParameter1f(this->turbDensityParam, this->turbulanceDensity);

	// Set the colour texture sampler to be noise
	cgGLSetTextureParameter(this->texSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
}