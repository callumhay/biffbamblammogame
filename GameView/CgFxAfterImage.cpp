#include "CgFxAfterImage.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/FBObj.h"

const std::string CgFxAfterImage::AFTERIMAGE_TECHNIQUE_NAME = "AfterImage";
const float CgFxAfterImage::AFTERIMAGE_BLURSTRENGTH_DEFAULT	= 0.64f;

CgFxAfterImage::CgFxAfterImage(FBObj* currFrameFBO) :
CgFxPostProcessingEffect(GameViewConstants::GetInstance()->CGFX_AFTERIMAGE_SHADER, currFrameFBO), 
prevFrameFBO(NULL), blurStrength(CgFxAfterImage::AFTERIMAGE_BLURSTRENGTH_DEFAULT) {

	// Create cleared temporary buffers
	this->prevFrameFBO = new FBObj(sceneFBO->GetFBOTexture()->GetWidth(), 
															   sceneFBO->GetFBOTexture()->GetHeight(), 
														     sceneFBO->GetFBOTexture()->GetFilter(),
														     FBObj::NoAttachment);
	this->prevFrameFBO->BindFBObj();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	this->prevFrameFBO->UnbindFBObj();

	this->currTechnique = this->techniques[CgFxAfterImage::AFTERIMAGE_TECHNIQUE_NAME];

	// Setup the CgFx parameters
	this->blurStrengthParam			= cgGetNamedEffectParameter(this->cgEffect, "BlurStrength");
	this->currFrameSamplerParam	= cgGetNamedEffectParameter(this->cgEffect, "CurrFrameSampler");

	debug_cg_state();
}

CgFxAfterImage::~CgFxAfterImage() {
	delete this->prevFrameFBO;
	this->prevFrameFBO = NULL;
}

void CgFxAfterImage::Draw(int screenWidth, int screenHeight, double dT) {
	// Step 0: Establish uniform parameter(s)
	cgGLSetTextureParameter(this->currFrameSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());
	cgGLSetParameter1f(this->blurStrengthParam, this->blurStrength);

	this->prevFrameFBO->BindFBObj();

	// Step 1: Draw a fullscreen quad with the previous frame rendered onto it
	this->prevFrameFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	// Step 2: Draw a fullscreen quad with the shader effect applied
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);

	this->prevFrameFBO->UnbindFBObj();
	this->prevFrameFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	debug_cg_state();
}