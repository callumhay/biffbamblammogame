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
	static const std::string BASIC_TECHNIQUE_NAME;
	static const std::string GHOSTBALL_TECHNIQUE_NAME;
	static const std::string SMOKESPRITE_TECHNIQUE_NAME;
	static const std::string FIRESPRITE_TECHNIQUE_NAME;

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
	GLint noiseTexID; 
	Texture2D* maskTex;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxVolumetricEffect();
	virtual ~CgFxVolumetricEffect();

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
};

#endif