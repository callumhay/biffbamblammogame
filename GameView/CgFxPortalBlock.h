/**
 * CgFxPortalBlock.h
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

#ifndef __CGFXPORTALBLOCK_H__
#define __CGFXPORTALBLOCK_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Colour.h"

class CgFxPortalBlock : public CgFxAbstractMaterialEffect {
public:
	static const char* PORTAL_BLOCK_TECHNIQUE_NAME;

	CgFxPortalBlock();
	~CgFxPortalBlock();

    void SetAlphaMultiplier(float a) {
        this->alphaMultiplier = a;
    }
	void SetFBOTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}
	void SetWarpAmountParam(float amt) {
		this->warpAmount = amt;
	}
	void SetIndexOfRefraction(float eta) {
		this->indexOfRefraction = eta;
	}
	void SetScale(float s) {
		this->scale = s;
	}
	void SetFrequency(float f) {
		this->freq = f;
	}
	void SetFlowDirection(const Vector3D& v) {
		this->flowDir = v;
	}
	void AddToTimer(double dT) {
		this->timer += dT;
	}

private:
	// CG Portal Block parameters
    CGparameter worldITMatrixParam;
    CGparameter wvpMatrixParam;
    CGparameter worldMatrixParam;
    CGparameter viewInvMatrixParam;

    CGparameter alphaMultiplierParam;
	CGparameter sceneSamplerParam;
	CGparameter indexOfRefactionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	CGparameter noiseSamplerParam;
	CGparameter timerParam;
	CGparameter noiseScaleParam;
	CGparameter noiseFreqParam;
	CGparameter noiseFlowDirParam;

	// Actual values for parameters
    float alphaMultiplier;
	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;
	double timer;
	float scale, freq;
	Vector3D flowDir;
	GLint noiseTexID;

    void SetupBeforePasses(const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(CgFxPortalBlock);
};

#endif // __CGFXPORTALBLOCK_H__