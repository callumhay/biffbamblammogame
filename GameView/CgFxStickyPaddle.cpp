/**
 * CgFxStickyPaddle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxStickyPaddle.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Texture3D.h"

const char* CgFxStickyPaddle::STICKYPADDLE_TECHNIQUE_NAME = "StickyPaddle";

CgFxStickyPaddle::CgFxStickyPaddle(MaterialProperties* properties) :
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_STICKYPADDLE_SHADER, properties),
timerParam(NULL), displacementParam(NULL), speedParam(NULL), waveScaleParam(NULL), refractScaleParam(NULL),
sceneWidthParam(NULL), sceneHeightParam(NULL), sceneSamplerParam(NULL),
timer(0.0f), displacement(0.1f), speed(1.6f), waveScale(3.0f), refractScale(10.0f), noiseScale(3.0f), 
sceneTex(NULL) {

	assert(properties->materialType == MaterialProperties::MATERIAL_STICKYGOO_TYPE);
	assert(properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE);

	// Set the technique to the only one available for this effect
	this->currTechnique = this->techniques[CgFxStickyPaddle::STICKYPADDLE_TECHNIQUE_NAME];
	assert(this->currTechnique != NULL);
	
	// Initialize all Cg parameters specific to this shader effect
	this->timerParam					= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->displacementParam		= cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->speedParam					= cgGetNamedEffectParameter(this->cgEffect, "Speed");
	this->waveScaleParam			= cgGetNamedEffectParameter(this->cgEffect, "WaveScale");
	this->refractScaleParam		= cgGetNamedEffectParameter(this->cgEffect, "RefractScale");
	this->noiseScaleParam			= cgGetNamedEffectParameter(this->cgEffect, "NoiseScale");
	this->sceneWidthParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	debug_cg_state();
}

CgFxStickyPaddle::~CgFxStickyPaddle() {
}

void CgFxStickyPaddle::SetupBeforePasses(const Camera& camera) {
	CgFxMaterialEffect::SetupBeforePasses(camera);

	this->timer = static_cast<float>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0f;

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->speedParam, this->speed);
	cgGLSetParameter1f(this->waveScaleParam, this->waveScale);
	cgGLSetParameter1f(this->refractScaleParam, this->refractScale);
	cgGLSetParameter1f(this->noiseScaleParam, this->noiseScale);

	// Set the scene texture
	assert(this->sceneTex != NULL);
	cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
	cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());	
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

	// Set the noise texture
	cgGLSetTextureParameter(this->texSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
}