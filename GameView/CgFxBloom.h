/**
 * CgFxBloom.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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