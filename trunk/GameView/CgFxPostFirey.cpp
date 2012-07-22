/**
 * CgFxPostFirey.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxPostFirey.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"

const char* CgFxPostFirey::POSTFIREY_TECHNIQUE_NAME = "PostFirey";

CgFxPostFirey::CgFxPostFirey(FBObj* outputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREEN_POSTFIREY_SHADER, NULL), resultFBO(outputFBO),
timerParam(NULL), frequencyParam(NULL), fadeParam(NULL), sceneSamplerParam(NULL),
freq(1.25f), timer(0.0f), fade(1.0f), scale(0.25f) {
	
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostFirey::POSTFIREY_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->timerParam				= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->frequencyParam			= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->fadeParam					= cgGetNamedEffectParameter(this->cgEffect, "Fade");
	this->scaleParam				= cgGetNamedEffectParameter(this->cgEffect, "Scale");

	// Initialize CG sampler parameters
	this->noiseSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxPostFirey::~CgFxPostFirey() {
}

void CgFxPostFirey::Draw(int screenWidth, int screenHeight, double dT) {
	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam,     this->timer);
	cgGLSetParameter1f(this->scaleParam,     this->scale);
	cgGLSetParameter1f(this->frequencyParam, this->freq);
	cgGLSetParameter1f(this->fadeParam,      this->fade);

	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
}