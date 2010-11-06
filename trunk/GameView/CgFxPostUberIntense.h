/**
 * CgFxPostUberIntense.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXPOSTUBERINTENSE_H__
#define __CGFXPOSTUBERINTENSE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen intense shader - Makes the whole screen pulsing and intense; happens
 * when the player is in ball camera mode and also has the uber ball active.
 */
class CgFxPostUberIntense : public CgFxPostProcessingEffect {

public:
	CgFxPostUberIntense(FBObj* outputFBO);
	~CgFxPostUberIntense();

	void Draw(int screenWidth, int screenHeight, double dT);

	inline void SetColour(const Colour& c) {
		this->colour = c;
	}
	inline void SetOutputFBO(FBObj* renderOut) {
		this->resultFBO = renderOut;
	}
	inline void SetFadeAlpha(float alpha) {
		this->fade = alpha;
	}

private:
	static const char* POSTUBERINTENSE_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;
	CGparameter colourParam;

	CGparameter sceneSamplerParam;

	float timer;
	float frequency;
	float fade;
	Colour colour;
	FBObj* resultFBO;
};

#endif // __CGFXPOSTUBERINTENSE_H__