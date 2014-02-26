/**
 * CgFxPostRefract.h
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
	static const char* NORMAL_TEXTURE_WITH_OVERLAY_TECHNIQUE_NAME;
	static const char* NORMAL_TEXTURE_WITH_NOISE_TECHNIQUE_NAME;

	CgFxPostRefract();
	~CgFxPostRefract();

	void SetFBOTexture(const Texture2D* tex);
	void SetNormalTexture(const Texture2D* tex);
	void SetOverlayTexture(const Texture2D* tex);
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
	CGparameter overlaySamplerParam;
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
	const Texture2D* overlayTex;
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
 * Sets the overlay texture to the given one - only applies to the normal with overlay technique.
 */
inline void CgFxPostRefract::SetOverlayTexture(const Texture2D* tex) {
	this->overlayTex = tex;
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