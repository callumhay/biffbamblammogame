/**
 * CgFxVolumetricEffect.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __CGFXVOLUMETRICEFFECT_H__
#define __CGFXVOLUMETRICEFFECT_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Vector.h"

class Camera;
class Texture2D;

class CgFxVolumetricEffect : public CgFxTextureEffectBase {
public:
	static const char* BASIC_TECHNIQUE_NAME;
	static const char* GHOSTBALL_TECHNIQUE_NAME;
	static const char* SMOKESPRITE_TECHNIQUE_NAME;
    static const char* CLOUDYSPRITE_TECHNIQUE_NAME;
	static const char* FIRESPRITE_TECHNIQUE_NAME;

	CgFxVolumetricEffect();
	~CgFxVolumetricEffect();

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

	// Timer parameter
	CGparameter timerParam;

	// Tweak-able params
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
	const Texture2D* maskTex;

	DISALLOW_COPY_AND_ASSIGN(CgFxVolumetricEffect);
};

#endif