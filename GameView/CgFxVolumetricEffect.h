#ifndef __CGFXVOLUMETRICEFFECT_H__
#define __CGFXVOLUMETRICEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

class Camera;

/**
 * Represents CgFx CelShader material.
 */
class CgFxVolumetricEffect : public CgFxEffectBase {

private:
	// Constants used for loading the CelShading effect
	static const std::string BASIC_TECHNIQUE_NAME;
	
	// CG Transform params
	CGparameter worldITMatrixParam;
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;
	CGparameter viewInvMatrixParam;

	// Noise texture sampler param
	CGparameter noiseSamplerParam;

	// Timer paramter
	CGparameter timerParam;

	// Tweakable params
	CGparameter scaleParam;
	CGparameter freqParam;
	CGparameter flowDirectionParam;
	CGparameter colourParam;
	CGparameter fadeExpParam;
	CGparameter constAmtParam;
	CGparameter alphaMultParam;

	// Actual values to be used for parameters in the shader
	double timer;
	float scale, freq, fadeExponent, constAmt, alphaMultiplier;
	Colour colour;
	Vector3D flowDir;
	GLint noiseTexID;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxVolumetricEffect();
	virtual ~CgFxVolumetricEffect();

	void SetScale(float s) {
		this->scale = s;
	}
	void SetFrequency(float f) {
		this->freq = f;
	}
	void SetFadeExponent(float f) {
		this->fadeExponent = f;
	}
	void SetConstantFactor(float c) {
		this->constAmt = c;
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
	void AddToTimer(double dT) {
		this->timer += dT;
	}
};

#endif