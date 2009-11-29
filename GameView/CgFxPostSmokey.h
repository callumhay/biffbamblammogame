#ifndef __CGFXPOSTSMOKEY_H__
#define __CGFXPOSTSMOKEY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen smokeyness - Makes the whole screen foggy/smokey; happens
 * when the player is in ball camera mode and also has the ghost ball active.
 */
class CgFxPostSmokey : public CgFxPostProcessingEffect {

public:
	CgFxPostSmokey(FBObj* inputFBO, FBObj* outputFBO);
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
	static const std::string POSTSMOKEY_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter displacementParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;

	float timer;
	float scale;
	float frequency;
	float displacement;
	float fade;
	Colour colour;
	FBObj* resultFBO;

};
#endif