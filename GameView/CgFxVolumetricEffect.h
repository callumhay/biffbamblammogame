/**
 * CgFxVolumetricEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXVOLUMETRICEFFECT_H__
#define __CGFXVOLUMETRICEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

class Camera;
class Texture2D;

/**
 * Represents CgFx CelShader material.
 */
class CgFxVolumetricEffect : public CgFxEffectBase {
public:
	// Constants used for loading the CelShading effect
	static const char* BASIC_TECHNIQUE_NAME;
	static const char* GHOSTBALL_TECHNIQUE_NAME;
	static const char* SMOKESPRITE_TECHNIQUE_NAME;
	static const char* FIRESPRITE_TECHNIQUE_NAME;

	CgFxVolumetricEffect();
	~CgFxVolumetricEffect();

	void SetMaskTexture(Texture2D* tex) {
		this->maskTex = tex;
	}

	void SetScale(float s) {
		this->scale = s;
	}
	void SetFrequency(float f) {
		this->freq = f;
	}
	void SetFadeExponent(float f) {
		this->fadeExponent = f;
	}
	void SetColour(const Colour& c) {
		this->colour = c;
	}
	void SetFlowDirection(const Vector3D& v) {
		this->flowDir = v;
	}
	void SetAlphaMultiplier(float a) {
		this->alphaMultiplier = a;
	}

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	// CG Transform params
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	CGparameter noiseSamplerParam;	// Noise texture sampler param
	CGparameter maskSamplerParam;		// Mask texture

	// Timer paramter
	CGparameter timerParam;

	// Tweakable params
	CGparameter scaleParam;
	CGparameter freqParam;
	CGparameter flowDirectionParam;
	CGparameter colourParam;
	CGparameter fadeExpParam;
	CGparameter alphaMultParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, fadeExponent, alphaMultiplier;
	Colour colour;
	Vector3D flowDir;
	GLuint noiseTexID; 
	Texture2D* maskTex;

	DISALLOW_COPY_AND_ASSIGN(CgFxVolumetricEffect);
};

#endif