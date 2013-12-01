/**
 * CgFxCloudEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXCLOUDEFFECT_H__
#define __CGFXCLOUDEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

class Camera;
class Texture2D;

class CgFxCloudEffect : public CgFxTextureEffectBase {
public:
	static const char* BASIC_TECHNIQUE_NAME;

	CgFxCloudEffect();
	~CgFxCloudEffect();

	void SetTexture(const Texture2D* tex) {
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
    void SetLightPos(const Point3D& p) {
        this->lightPos = p;
    }
    void SetAttenuation(float atten) {
        this->attenuation = atten;
    }

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	// CG Transform params
	CGparameter wvpMatrixParam;

	CGparameter noiseSamplerParam;	// Noise texture sampler param
	CGparameter maskSamplerParam;   // Mask texture

	// Timer parameter
	CGparameter timerParam;

	// Tweak-able params
	CGparameter scaleParam;
	CGparameter freqParam;
	CGparameter flowDirectionParam;
	CGparameter colourParam;
	CGparameter fadeExpParam;
    CGparameter lightPosParam;
    CGparameter attenuationParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, fadeExponent;
	Colour colour;
	Vector3D flowDir;
	GLuint noiseTexID; 
	const Texture2D* maskTex;
    Point3D lightPos;
    float attenuation;

	DISALLOW_COPY_AND_ASSIGN(CgFxCloudEffect);
};

#endif // __CGFXCLOUDEFFECT_H__