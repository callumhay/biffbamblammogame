/**
 * CgFxBloom.h
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

#ifndef __CGFXBLOOM_H__
#define __CGFXBLOOM_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;
class CgFxGaussianBlur;

class CgFxBloom : public CgFxPostProcessingEffect {
public:
	CgFxBloom(FBObj* sceneFBO, CgFxGaussianBlur* blurEffect);
	~CgFxBloom();

	void Draw(int screenWidth, int screenHeight, double dT);

    inline void SetMenuBloomSettings() {
        this->SetHighlightThreshold(0.4f);
        this->SetSceneIntensity(0.70f);
        this->SetGlowIntensity(0.3f);
        this->SetHighlightIntensity(0.1f);
    }

	inline void SetHighlightThreshold(float hlgtThreshold) {
		this->highlightThreshold = hlgtThreshold;
	}
	inline void SetSceneIntensity(float sceneIntensity) {
		this->sceneIntensity = sceneIntensity;
	}
	inline void SetGlowIntensity(float glowIntensity) {
		this->glowIntensity = glowIntensity;
	}
	inline void SetHighlightIntensity(float hlgtIntensity) {
		this->highlightIntensity = hlgtIntensity;
	}
    inline FBObj* GetInitialBloomFilterFBO() const {
        return this->bloomFilterFBO;
    }

private:
    static const char* BLOOM_FILTER_TECHNIQUE_NAME;
    static const char* BLOOM_COMPOSITION_TECHNIQUE_NAME;

    static const float DEFAULT_HIGHLIGHT_THRESHOLD;
    static const float DEFAULT_SCENE_INTENSITY;
    static const float DEFAULT_GLOW_INTENSITY;
    static const float DEFAULT_HIGHLIGHT_INTENSITY;

    // CG parameters
    CGparameter sceneSamplerParam;
    CGparameter brightBlurSamplerParam;

    CGparameter brightBlurSamplerMip3Param;
    CGparameter brightBlurSamplerMip4Param;

    CGparameter sceneWidthParam;
    CGparameter sceneHeightParam;

    CGparameter highlightThresholdParam;
    CGparameter sceneIntensityParam;
    CGparameter glowIntensityParam;
    CGparameter highlightIntensityParam;

    CgFxGaussianBlur* blurEffect; // Not owned by this
    FBObj* bloomFilterFBO;

    float highlightThreshold, sceneIntensity, glowIntensity, highlightIntensity;
};
#endif