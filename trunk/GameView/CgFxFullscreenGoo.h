#ifndef __CGFXFULLSCREENGOO_H__
#define __CGFXFULLSCREENGOO_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen goo - occurs when the player has activated paddle camera mode and
 * also has the sticky paddle powerup active.
 */
class CgFxFullscreenGoo : public CgFxPostProcessingEffect {
protected:
	static const std::string MASK_SPLATTER_TECHNIQUE_NAME;
	static const std::string NOMASK_SPLATTER_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter displacementParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter maskSamplerParam;
	CGparameter sceneSamplerParam;

	float timer;
	float scale;
	float frequency;
	float displacement;
	float fade;

	Colour colour;
	Texture* maskTex;

	FBObj* resultFBO;

public:
	CgFxFullscreenGoo(FBObj* outputFBO);
	virtual ~CgFxFullscreenGoo();

	virtual void Draw(int screenWidth, int screenHeight, double dT);

	void SetColour(const Colour& c);
	void SetMask(const std::string& texMaskFilepath);
	void SetOutputFBO(FBObj* renderOut);
	void SetFadeAlpha(float alpha);
	void SetDisplacement(float d);
	void SetScale(float s);
};

inline void CgFxFullscreenGoo::SetColour(const Colour& c) {
	this->colour = c;
}
inline void CgFxFullscreenGoo::SetOutputFBO(FBObj* renderOut) {
	this->resultFBO = renderOut;
}
inline void CgFxFullscreenGoo::SetFadeAlpha(float alpha) {
	this->fade = alpha;
}
inline void CgFxFullscreenGoo::SetDisplacement(float d) {
	this->displacement = d;
}
inline void CgFxFullscreenGoo::SetScale(float s) {
	this->scale = s;
}

#endif