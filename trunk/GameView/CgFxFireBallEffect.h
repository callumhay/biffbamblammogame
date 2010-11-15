/**
 * CgFxFireBallEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXFIREBALLEFFECT_H__
#define __CGFXFIREBALLEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"


class CgFxFireBallEffect : public CgFxEffectBase {
public:
	// Constants used for loading the CelShading effect
	static const char* BASIC_TECHNIQUE_NAME;
	static const char* NO_DEPTH_WITH_MASK_TECHNIQUE_NAME;

	CgFxFireBallEffect();
	~CgFxFireBallEffect();

	void SetAlphaMultiplier(float a) {
		this->alphaMultiplier = a;
	}

	void SetMaskTexture(Texture2D* tex) {
		this->maskTex = tex;
	}

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	// CG Transform params
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;

	// Texture sampler params
	CGparameter noiseSamplerParam;
	CGparameter maskSamplerParam;

	// Timer paramter
	CGparameter timerParam;

	// Tweakable params
	CGparameter scaleParam;
	CGparameter freqParam;
	CGparameter flowDirectionParam;
	CGparameter alphaMultParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, alphaMultiplier;
	Vector3D flowDir;
	GLuint noiseTexID; 
	Texture2D* maskTex;

	DISALLOW_COPY_AND_ASSIGN(CgFxFireBallEffect);
};


#endif // __CGFXFIREBALLEFFECT_H__