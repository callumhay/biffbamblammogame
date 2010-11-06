/**
 * CgFxPostSmokey.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CGFXPOSTSMOKEY_H__
#define __CGFXPOSTSMOKEY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen smokeyness - Makes the whole screen foggy/smokey; happens
 * when the player is in ball camera mode and also has the ghost ball active.
 */
class CgFxPostSmokey : public CgFxPostProcessingEffect {

public:
	CgFxPostSmokey(FBObj* outputFBO);
	~CgFxPostSmokey();

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
	static const char* POSTSMOKEY_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;

	float timer;
	float scale;
	float frequency;
	float fade;
	Colour colour;
	FBObj* resultFBO;

};
#endif