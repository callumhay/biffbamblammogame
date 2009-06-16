#include "CgFxInkSplatter.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

#include "../ResourceManager.h"

const std::string CgFxInkSplatter::INK_SPLATTER_TECHNIQUE_NAME = "InkSplatter";

CgFxInkSplatter::CgFxInkSplatter(FBObj* sceneFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_INKSPLATTER_SHADER, sceneFBO),
timerParam(NULL), scaleParam(NULL), frequencyParam(NULL), displacementParam(NULL), fadeParam(NULL),
inkColourParam(NULL), noiseSamplerParam(NULL), inkSplatterSamplerParam(NULL), sceneSamplerParam(NULL),
scale(2.0f), frequency(0.01f), displacement(0.04f), inkColour(GameViewConstants::GetInstance()->INK_BLOCK_COLOUR),
inkSplatterTex(NULL), isActivated(false), timer(0.0f) {
	
	// Make sure the technique is set
	this->currTechnique = this->techniques[CgFxInkSplatter::INK_SPLATTER_TECHNIQUE_NAME];

	// Initialize CG simple float parameters
	this->timerParam					= cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->scaleParam					= cgGetNamedEffectParameter(this->cgEffect, "Scale");
	this->frequencyParam			= cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->displacementParam		= cgGetNamedEffectParameter(this->cgEffect, "Displacement");
	this->fadeParam						= cgGetNamedEffectParameter(this->cgEffect, "Fade");
	this->inkColourParam			= cgGetNamedEffectParameter(this->cgEffect, "InkColour");

	// Initialize the ink splatter texture
	this->inkSplatterTex	= ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER, Texture::Trilinear, GL_TEXTURE_2D);
	assert(this->inkSplatterTex != NULL);

	// Initialize CG sampler parameters
	this->noiseSamplerParam				= cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->inkSplatterSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "InkSplatterSampler");
	this->sceneSamplerParam				= cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

	debug_cg_state();
}

CgFxInkSplatter::~CgFxInkSplatter() {
	// Free the splatter texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->inkSplatterTex);
	assert(success);
}

/**
 * Activates the ink splatter effect.
 */
void CgFxInkSplatter::Activate() {
	if (this->isActivated) {
		// Setup the animation for fading in and out the ink splatter
		// such that it is a secondary ink splatter to occur
		std::vector<double> timeVals;
		timeVals.reserve(3);
		timeVals.push_back(0.0);
		timeVals.push_back(4.0);
		timeVals.push_back(9.0);

		std::vector<float> fadeVals;
		fadeVals.reserve(3);
		fadeVals.push_back(1.0f);	// Start the full splatter immediately
		fadeVals.push_back(1.0f); // Keep splatter until here
		fadeVals.push_back(0.0f); // Fade splatter

		this->fadeAnim.SetLerp(timeVals, fadeVals);		
	}
	else {
		// Setup the animation for fading in and out the ink splatter
		// such that it is the first ink splatter to occur
		std::vector<double> timeVals;
		timeVals.reserve(5);
		timeVals.push_back(0.0);
		timeVals.push_back(0.79);
		timeVals.push_back(0.8);
		timeVals.push_back(4.0);
		timeVals.push_back(9.0);

		std::vector<float> fadeVals;
		fadeVals.reserve(5);
		fadeVals.push_back(0.0f);	// Don't have any splatter to begin
		fadeVals.push_back(0.0f);	// Makes sure there's no interpolation
		fadeVals.push_back(1.0f);	// Start the full splatter immediately
		fadeVals.push_back(1.0f); // Keep splatter until here
		fadeVals.push_back(0.0f); // Fade splatter

		this->fadeAnim.SetLerp(timeVals, fadeVals);
		this->isActivated = true;
	}
}



void CgFxInkSplatter::Draw(int screenWidth, int screenHeight, double dT) {
	// If this effect is not activated then leave
	if (!this->isActivated) {
		return;
	}

	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->frequencyParam, this->frequency);
	cgGLSetParameter1f(this->fadeParam, this->fadeAnim.GetInterpolantValue());
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter3f(this->inkColourParam, this->inkColour.R(), this->inkColour.G(), this->inkColour.B());

	cgGLSetTextureParameter(this->inkSplatterSamplerParam, this->inkSplatterTex->GetTextureID());
	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);

	// The effect will be active as long as the timer is not complete
	this->isActivated = !this->fadeAnim.Tick(dT);
}