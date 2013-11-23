/**
 * CgFxFireBallEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXFIREBALLEFFECT_H__
#define __CGFXFIREBALLEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"


class CgFxFireBallEffect : public CgFxTextureEffectBase {
public:
	// Constants used for loading the CelShading effect
	static const char* BASIC_TECHNIQUE_NAME;
	static const char* NO_DEPTH_WITH_MASK_TECHNIQUE_NAME;

	CgFxFireBallEffect();
	~CgFxFireBallEffect();

    void SetTexture(const Texture2D* texture) {
        this->maskTex = texture;
    }

	void SetAlphaMultiplier(float a) {
		this->alphaMultiplier = a;
	}
	void SetBrightFireColour(const Colour& c) {
		this->brightFireColour = c;
	}
	void SetDarkFireColour(const Colour& c) {
		this->darkFireColour = c;
	}

	void SetScale(float scale) {
		this->scale = scale;
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
	CGparameter brightFireColourParam;
	CGparameter darkFireColourParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, alphaMultiplier;
	Vector3D flowDir;
	GLuint noiseTexID; 
	const Texture2D* maskTex;

	Colour brightFireColour;
	Colour darkFireColour;

	DISALLOW_COPY_AND_ASSIGN(CgFxFireBallEffect);
};


#endif // __CGFXFIREBALLEFFECT_H__