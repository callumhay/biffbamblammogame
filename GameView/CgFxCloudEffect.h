/**
 * CgFxCloudEffect.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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