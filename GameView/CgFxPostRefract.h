#ifndef __CGFXPOSTREFRACT_H__
#define __CGFXPOSTREFRACT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"

class Camera;

/**
 * Represents CgFx Post Refraction material.
 */
class CgFxPostRefract : public CgFxEffectBase {
public:
	static const char* BASIC_TECHNIQUE_NAME;
	static const char* NORMAL_TEXTURE_TECHNIQUE_NAME;
	static const char* NORMAL_TEXTURE_WITH_NOISE_TECHNIQUE_NAME;

	CgFxPostRefract();
	~CgFxPostRefract();

	void SetFBOTexture(const Texture2D* tex);
	void SetNormalTexture(const Texture2D* tex);
	void SetWarpAmountParam(float amt);
	void SetIndexOfRefraction(float eta);

	void SetScale(float s) {
		this->scale = s;
	}
	void SetFrequency(float f) {
		this->freq = f;
	}
	void AddToTimer(double dT) {
		this->timer += dT;
	}

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	// CG Transform params
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	// CG Refract params
	CGparameter sceneSamplerParam;
	CGparameter normalSamplerParam;
	CGparameter noiseSamplerParam;
	CGparameter indexOfRefactionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	CGparameter timerParam;
	CGparameter noiseScaleParam;
	CGparameter noiseFreqParam;

	// Actual values for parameters
	float indexOfRefraction, warpAmount;
	double timer;
	float scale, freq;	
	const Texture2D* sceneTex;
	const Texture2D* normalTex;
};

/**
 * Set the FBO texture (the texture where the scene thus far
 * has been rendered to).
 */
inline void CgFxPostRefract::SetFBOTexture(const Texture2D* tex) {
	this->sceneTex = tex;
}

/**
 * Set the normal texture to the one given - this only applies to the normal texture
 * post-refraction effect technique.
 */
inline void CgFxPostRefract::SetNormalTexture(const Texture2D* tex) {
	this->normalTex = tex;
}

/**
 * Set the average (doesn't have to be perfect) amount to warp
 * the lookup into the scene texture.
 */
inline void CgFxPostRefract::SetWarpAmountParam(float amt) {
	this->warpAmount = amt;
}
/**
 * Set the index of refraction for the background.
 */
inline void CgFxPostRefract::SetIndexOfRefraction(float eta) {
	this->indexOfRefraction = eta;
}

#endif