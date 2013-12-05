/**
 * CgFxGaussianBlur.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXGAUSSIANBLUR_H__
#define __CGFXGAUSSIANBLUR_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxGaussianBlur : public CgFxPostProcessingEffect {
public:
	enum BlurType { Kernel3x3, Kernel5x5, Kernel7x7 };

	CgFxGaussianBlur(BlurType blurType, FBObj* sceneFBO);
	~CgFxGaussianBlur();

	void SetPoisonBlurAnimation(bool on);
    void Tick(double dT);

	void SetBlurType(BlurType type);
	void Draw(int screenWidth, int screenHeight);

    void SetSigma(float sigma) { this->sigma = sigma; }

private:
	static const char* GAUSSIANBLUR_3x3_TECHNIQUE_NAME;
	static const char* GAUSSIANBLUR_5x5_TECHNIQUE_NAME;
	static const char* GAUSSIANBLUR_7x7_TECHNIQUE_NAME;
	
	FBObj* tempFBO;
	BlurType blurType;
    double sigma;

	// CG parameters
    CGparameter sceneSamplerParam;
	CGparameter blurSizeHorizontalParam;
    CGparameter blurSizeVerticalParam;
	CGparameter sigmaParam;

    bool isPoisonBlurActive;
	AnimationMultiLerp<float> poisonBlurAnim;

    void Draw(int, int, double) { assert(false); }

    DISALLOW_COPY_AND_ASSIGN(CgFxGaussianBlur);
};

inline void CgFxGaussianBlur::Tick(double dT) {
    // Check for any animations
    if (this->isPoisonBlurActive) {
        if (this->poisonBlurAnim.Tick(dT)) {
            this->isPoisonBlurActive = false;
        }
    }
}

#endif