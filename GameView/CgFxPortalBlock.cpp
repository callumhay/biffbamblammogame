/**
 * CgFxPortalBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxPortalBlock.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"

const char* CgFxPortalBlock::PORTAL_BLOCK_TECHNIQUE_NAME = "PortalBlock";

// Default constructor
CgFxPortalBlock::CgFxPortalBlock(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PORTALBLOCK_SHADER, properties), 
indexOfRefraction(1.55f), scale(0.05f), freq(1.0f), flowDir(0, 0, 1),
timer(0.0), warpAmount(60.0f), sceneTex(NULL), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {
	assert(properties->materialType == MaterialProperties::MATERIAL_PORTAL_TYPE);

	// Set the technique
	this->currTechnique = this->techniques[PORTAL_BLOCK_TECHNIQUE_NAME];

	// Refraction parameters
	this->indexOfRefactionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam			  = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam			= cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");

	this->noiseScaleParam				= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->noiseFreqParam				= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->noiseFlowDirParam			= cgGetNamedEffectParameter(this->cgEffect, "FlowDir");
	this->timerParam						= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->noiseSamplerParam			= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");

	// The rendered scene background texture
	this->sceneSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	
	debug_cg_state();
}

CgFxPortalBlock::~CgFxPortalBlock() {
}

void CgFxPortalBlock::SetupBeforePasses(const Camera& camera) {
	// Refraction property setup
	cgGLSetParameter1f(this->indexOfRefactionParam, this->indexOfRefraction);
	cgGLSetParameter1f(this->warpAmountParam,  this->warpAmount);

	// Set the scene texture
	cgGLSetParameter1f(this->sceneWidthParam,  this->sceneTex->GetWidth());
	cgGLSetParameter1f(this->sceneHeightParam, this->sceneTex->GetHeight());
	assert(this->sceneTex != NULL);
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->noiseScaleParam, this->scale);
	cgGLSetParameter1f(this->noiseFreqParam, this->freq);
	cgGLSetParameter3fv(this->noiseFlowDirParam, this->flowDir.begin());
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);

	CgFxMaterialEffect::SetupBeforePasses(camera);
}