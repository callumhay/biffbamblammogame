/**
 * CgFxPostUberIntense.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "CgFxPostUberIntense.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"

const char* CgFxPostUberIntense::POSTUBERINTENSE_TECHNIQUE_NAME = "PostUberIntense";

CgFxPostUberIntense::CgFxPostUberIntense(FBObj* outputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREEN_UBERINTENSE_SHADER, sceneFBO), resultFBO(outputFBO),
timerParam(NULL), frequencyParam(NULL), fadeParam(NULL), colourParam(NULL), sceneSamplerParam(NULL),
frequency(1.5f), colour(GameModelConstants::GetInstance()->UBER_BALL_COLOUR), timer(0.0f), fade(1.0f) {
	
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostUberIntense::POSTUBERINTENSE_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->timerParam					= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->frequencyParam			= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->fadeParam						= cgGetNamedEffectParameter(this->cgEffect, "Fade");
	this->colourParam					= cgGetNamedEffectParameter(this->cgEffect, "Colour");

	// Initialize CG sampler parameters
	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxPostUberIntense::~CgFxPostUberIntense() {
}

void CgFxPostUberIntense::Draw(int screenWidth, int screenHeight, double dT) {
	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->frequencyParam, this->frequency);
	cgGLSetParameter1f(this->fadeParam, this->fade);
	cgGLSetParameter3fv(this->colourParam, this->colour.begin());

	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
}