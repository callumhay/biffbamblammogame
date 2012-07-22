/**
 * CgFxAfterImage.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXAFTERIMAGE_H__
#define __CGFXAFTERIMAGE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxAfterImage : public CgFxPostProcessingEffect {
private:
	static const std::string AFTERIMAGE_TECHNIQUE_NAME;

	FBObj* prevFrameFBO;
	FBObj* outputFBO;
	float blurStrength;

	// CgFx Parameters
	CGparameter blurStrengthParam;
	CGparameter currFrameSamplerParam;

public:
	static const float AFTERIMAGE_BLURSTRENGTH_DEFAULT;
	CgFxAfterImage(FBObj* currFrameFBO, FBObj* outputFBO);
	virtual ~CgFxAfterImage();

	void Draw(int screenWidth, int screenHeight, double dT);
	
	/**
	 * Set the blur amount for the after image,
	 * the lower the value the stronger the blur/afterimage -
	 * where 0 would be completely the afterimage and 1 would be
	 * no afterimage at all.
	 */
	inline void SetBlurAmount(float blur) {
		blur = std::max<float>(blur, 0.0f);
		blur = std::min<float>(blur, 1.0f);
		this->blurStrength = blur;
	}
};

#endif