#ifndef __CGFXAFTERIMAGE_H__
#define __CGFXAFTERIMAGE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxAfterImage : public CgFxPostProcessingEffect {
private:
	static const std::string AFTERIMAGE_TECHNIQUE_NAME;

	FBObj* prevFrameFBO;
	float blurStrength;

	// CgFx Parameters
	CGparameter blurStrengthParam;
	CGparameter currFrameSamplerParam;

public:
	static const float AFTERIMAGE_BLURSTRENGTH_DEFAULT;
	CgFxAfterImage(FBObj* currFrameFBO);
	virtual ~CgFxAfterImage();

	virtual void Draw(int screenWidth, int screenHeight, double dT);
	
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