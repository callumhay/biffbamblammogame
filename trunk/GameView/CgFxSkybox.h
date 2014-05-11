/**
 * CgFxSkybox.h
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

#ifndef __CGFXDECOSKYBOX_H__
#define __CGFXDECOSKYBOX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class Texture;

class CgFxSkybox : public CgFxEffectBase {
public:
    CgFxSkybox(Texture* fgSkyTex, Texture *bgSkyTex);
    ~CgFxSkybox();

    void SetMoveFrequency(float freq) { this->moveFreq = freq; }
    void SetFGScale(float scale) { this->fgScale = scale; }
    void SetAlpha(float alpha) { this->alpha = alpha; }
    void SetUVTranslation(float s, float t) {
        this->uvTranslation[0] = s;
        this->uvTranslation[1] = t;
    }

    Texture* GetBGSkyTexture() const { return this->bgSkyTex; }

protected:
    void SetupBeforePasses(const Camera& camera);

private:
	static const char* SKYBOX_TECHNIQUE_NAME;

	// CgFx parameters
	CGparameter wvpMatrixParam;
	CGparameter worldMatrixParam;

    CGparameter fgScaleParam;
	CGparameter twinkleFreqParam;
    CGparameter moveFreqParam;
    CGparameter noiseScaleParam;
	CGparameter timerParam;
    CGparameter alphaParam;
	CGparameter viewDirParam;
    CGparameter uvTranslateParam;

	CGparameter fgSkySamplerParam;
	CGparameter bgSkySamplerParam;

	// Stored values for parameters
	float timer, twinkleFreq, moveFreq, noiseScale, fgScale, alpha;
	Vector2D uvTranslation;
    GLint noiseTexID; 
	Texture* bgSkyTex;
    Texture* fgSkyTex;

	Vector3D viewDir;

    DISALLOW_COPY_AND_ASSIGN(CgFxSkybox);
};

#endif // __CGFXDECOSKYBOX_H__