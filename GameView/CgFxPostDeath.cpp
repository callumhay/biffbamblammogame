/**
 * CgFxPostDeath.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxPostDeath.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"

const char* CgFxPostDeath::POSTDEATH_TECHNIQUE_NAME = "PostDeath";

CgFxPostDeath::CgFxPostDeath(FBObj* inputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREEN_POSTDEATH_SHADER, inputFBO),
intensityParam(NULL), alphaParam(NULL), sceneSamplerParam(NULL), intensity(0.0f), alpha(1.0f) {

	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostDeath::POSTDEATH_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->intensityParam = cgGetNamedEffectParameter(this->cgEffect, "Intensity");
    this->alphaParam     = cgGetNamedEffectParameter(this->cgEffect, "Alpha");

	// Initialize CG sampler parameters
	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxPostDeath::~CgFxPostDeath() {
}

void CgFxPostDeath::Draw(int screenWidth, int screenHeight, double dT) {
    UNUSED_PARAMETER(dT);

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->intensityParam, this->intensity);
    cgGLSetParameter1f(this->alphaParam, this->alpha);

	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
    GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight, -1.0f);
	cgResetPassState(currPass);
}