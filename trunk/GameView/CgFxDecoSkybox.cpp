#include "CgFxDecoSkybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Texture.h"
#include "../BlammoEngine/Texture3D.h"

#include "../ResourceManager.h"

const std::string CgFxDecoSkybox::DECOSKYBOX_TECHNIQUE_NAME = "DecoSkybox";

CgFxDecoSkybox::CgFxDecoSkybox(Texture *skyTex) :
CgFxEffectBase(GameViewConstants::GetInstance()->CGFX_DECOSKYBOX_SHADER), skyTex(skyTex),
timer(0.0f), freq(0.025f), noiseTexID(Noise::GetInstance()->GetNoise3DTexture()->GetTextureID()) {
	assert(skyTex != NULL);

	// Set the technique
	this->currTechnique = this->techniques[DECOSKYBOX_TECHNIQUE_NAME];

	// Transform parameters
	this->wvpMatrixParam			= cgGetNamedEffectParameter(this->cgEffect, "ModelViewProjXf");
	this->worldMatrixParam		= cgGetNamedEffectParameter(this->cgEffect, "WorldXf");

	// Noise texture sampler param
	this->noiseSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "NoiseSampler");
	this->skySamplerParam   = cgGetNamedEffectParameter(this->cgEffect, "SkySampler");

	// Time and frequency paramters
	this->timerParam = cgGetNamedEffectParameter(this->cgEffect, "Timer");
	this->freqParam	 = cgGetNamedEffectParameter(this->cgEffect, "Freq");
	this->viewDirParam = cgGetNamedEffectParameter(this->cgEffect, "ViewDir");

	debug_cg_state();
}

CgFxDecoSkybox::~CgFxDecoSkybox() {
	// Release the skybox texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->skyTex);
	assert(success);
}

void CgFxDecoSkybox::SetupBeforePasses(const Camera& camera) {
	// Transform setup
	cgGLSetStateMatrixParameter(this->wvpMatrixParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	
	// Obtain the current model view and inverse view transforms
	float tempMVXfVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	Matrix4x4 tempMVXf(tempMVXfVals);
	Matrix4x4 invViewXf = camera.GetInvViewTransform();

	// Make sure that JUST the world transform is set
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(invViewXf.begin());
	glMultMatrixf(tempMVXf.begin());
	// Set the world transform parameter
	cgGLSetStateMatrixParameter(this->worldMatrixParam, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	glPopMatrix();

	cgGLSetParameter1f(this->freqParam, this->freq);
	double timeInSecs = static_cast<double>(BlammoTime::GetSystemTimeInMillisecs()) / 1000.0;
	cgGLSetParameter1f(this->timerParam, timeInSecs);

	Vector3D camDir = camera.GetNormalizedViewVector();
	cgGLSetParameter3f(this->viewDirParam, camDir[0], camDir[1], camDir[2]);

	// Set noise sampler
	cgGLSetTextureParameter(this->noiseSamplerParam, this->noiseTexID);
	// Set the sky sampler
	cgGLSetTextureParameter(this->skySamplerParam, this->skyTex->GetTextureID());
}