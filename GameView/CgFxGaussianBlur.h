#ifndef __CGFXGAUSSIANBLUR_H__
#define __CGFXGAUSSIANBLUR_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxGaussianBlur : public CgFxPostProcessingEffect {
public:
	enum BlurType { Kernel3x3, Kernel5x5, Kernel7x7 };

private:
	static const std::string GAUSSIANBLUR_3x3_TECHNIQUE_NAME;
	static const std::string GAUSSIANBLUR_5x5_TECHNIQUE_NAME;
	static const std::string GAUSSIANBLUR_7x7_TECHNIQUE_NAME;
	
	FBObj* tempFBO;
	BlurType blurType;

	// CG parameters
	CGparameter sceneSamplerParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;

	enum BlurAnimations { PoisonAnimation };
	std::map<BlurAnimations, AnimationMultiLerp<float>> blurAnims;

public:
	CgFxGaussianBlur(BlurType blurType, FBObj* sceneFBO);
	virtual ~CgFxGaussianBlur();

	void SetPoisonBlurAnimation(bool on);

	void SetBlurType(BlurType type);
	virtual void Draw(int screenWidth, int screenHeight, double dT);
};
#endif