/**
 * CgFxPostDeath.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXPOSTDEATH_H__
#define __CGFXPOSTDEATH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class CgFxPostDeath : public CgFxPostProcessingEffect {
public:
	CgFxPostDeath(FBObj* outputFBO);
	~CgFxPostDeath();

	void Draw(int screenWidth, int screenHeight, double dT);


    void SetAlpha(float a);
	void SetIntensity(float i);

private:
	static const char* POSTDEATH_TECHNIQUE_NAME;

	// Texture sampler params
	CGparameter sceneSamplerParam;

	// Tweakable params
	CGparameter intensityParam;
    CGparameter alphaParam;

	// Actual values to be used for parameters in the shader
	float intensity;
	float alpha;
    FBObj* resultFBO;

    
	DISALLOW_COPY_AND_ASSIGN(CgFxPostDeath);
};

inline void CgFxPostDeath::SetAlpha(float a) {
	this->alpha = a;
}

inline void CgFxPostDeath::SetIntensity(float i) {
	this->intensity = i;
}

#endif // __CGFXPOSTDEATH_H__