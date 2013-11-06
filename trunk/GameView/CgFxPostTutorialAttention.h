/**
 * CgFxPostTutorialAttention.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXPOSTTUTORIALATTENTION_H__
#define __CGFXPOSTTUTORIALATTENTION_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxPostTutorialAttention : public CgFxPostProcessingEffect {
public:
	CgFxPostTutorialAttention(FBObj* sceneFBO);
	~CgFxPostTutorialAttention();

    void SetAlpha(float alpha);
    void CrossFadeIn(double fadeInTime);
    void CrossFadeOut(double fadeOutTime);

	void Draw(int screenWidth, int screenHeight, double dT);

private:
	static const char* TECHNIQUE_NAME;
	
	FBObj* tempFBO;
    AnimationLerp<float> fadeAnim;

	// CG parameters
    CGparameter sceneSamplerParam;
	CGparameter blurSizeHorizontalParam;
    CGparameter blurSizeVerticalParam;
	CGparameter sigmaParam;
    CGparameter greyscaleFactorParam;

    DISALLOW_COPY_AND_ASSIGN(CgFxPostTutorialAttention);
};

#endif // __CGFXPOSTTUTORIALATTENTION_H__