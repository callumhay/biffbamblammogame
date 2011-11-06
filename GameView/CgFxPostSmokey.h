/**
 * CgFxPostSmokey.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2010
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
	static const char* POSTSMOKEY_TECHNIQUE_NAME;
	static const char* POST_ICY_TECHNIQUE_NAME;

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
	inline void SetScale(float scale) {
		this->scale = scale;
	}
	inline void SetFrequency(float freq) {
		this->frequency = freq;
	}

private:
	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;
	CGparameter overlaySamplerParam;

	float timer;
	float scale;
	float frequency;
	float fade;
	Colour colour;
	FBObj* resultFBO;
	Texture2D* overlayTex; 

};
#endif