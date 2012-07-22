/**
 * CgFxPostFirey.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXPOSTFIREY_H__
#define __CGFXPOSTFIREY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class CgFxPostFirey : public CgFxPostProcessingEffect {
public:
	CgFxPostFirey(FBObj* outputFBO);
	~CgFxPostFirey();

	void Draw(int screenWidth, int screenHeight, double dT);

	void SetColour(const Colour& c);
	void SetOutputFBO(FBObj* renderOut);
	void SetFadeAlpha(float alpha);

private:
	static const char* POSTFIREY_TECHNIQUE_NAME;

	// Texture sampler params
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;

	// Timer paramter
	CGparameter timerParam;

	// Tweakable params
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, fade, timer;
	GLuint noiseTexID;
	FBObj* resultFBO;

	DISALLOW_COPY_AND_ASSIGN(CgFxPostFirey);
};

inline void CgFxPostFirey::SetOutputFBO(FBObj* renderOut) {
	this->resultFBO = renderOut;
}
inline void CgFxPostFirey::SetFadeAlpha(float alpha) {
	this->fade = alpha;
}

#endif // __CGFXPOSTFIREY_H__