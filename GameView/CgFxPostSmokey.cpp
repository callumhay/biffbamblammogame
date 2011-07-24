/**
 * CgFxPostSmokey.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "CgFxPostSmokey.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameModelConstants.h"

const char* CgFxPostSmokey::POSTSMOKEY_TECHNIQUE_NAME = "PostSmokey";
const char* CgFxPostSmokey::POST_ICY_TECHNIQUE_NAME		= "PostIcy";

CgFxPostSmokey::CgFxPostSmokey(FBObj* outputFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_FULLSCREENSMOKEY_SHADER, sceneFBO), resultFBO(outputFBO),
timerParam(NULL), scaleParam(NULL), frequencyParam(NULL), fadeParam(NULL), colourParam(NULL), noiseSamplerParam(NULL), sceneSamplerParam(NULL),
scale(0.5f), frequency(0.5f), colour(GameModelConstants::GetInstance()->GHOST_BALL_COLOUR), timer(0.0f), fade(1.0f) {
	
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxPostSmokey::POSTSMOKEY_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->timerParam					= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->scaleParam					= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->frequencyParam			    = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->fadeParam						= cgGetNamedEffectParameter(this->cgEffect, "Fade");
	this->colourParam					= cgGetNamedEffectParameter(this->cgEffect, "Colour");

	// Initialize CG sampler parameters
	this->noiseSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->sceneSamplerParam		= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");
	this->overlaySamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "OverlaySampler");

	// The overlay texture is for the post-icy effect... get a semi-transparent frost texture
	this->overlayTex = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_FULLSCREEN_FROST, Texture::Trilinear));
	assert(this->overlayTex != NULL);

	debug_cg_state();
}

CgFxPostSmokey::~CgFxPostSmokey() {
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->overlayTex);
	assert(success);
}

void CgFxPostSmokey::Draw(int screenWidth, int screenHeight, double dT) {
	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->frequencyParam, this->frequency);
	cgGLSetParameter1f(this->fadeParam, this->fade);
	cgGLSetParameter3fv(this->colourParam, this->colour.begin());

	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	if (this->overlayTex != NULL) {
		cgGLSetTextureParameter(this->overlaySamplerParam, this->overlayTex->GetTextureID());
	}

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
}