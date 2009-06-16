#ifndef __CGFXINKSPLATTER_H__
#define __CGFXINKSPLATTER_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Class for handling the full screen shader effect of the ink block's
 * splatter across the screen.
 */
class CgFxInkSplatter : public CgFxPostProcessingEffect {
private:
	static const std::string INK_SPLATTER_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter displacementParam;
	CGparameter fadeParam;
	CGparameter inkColourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter inkSplatterSamplerParam;
	CGparameter sceneSamplerParam;

	float timer;
	float scale;
	float frequency;
	float displacement;

	Colour inkColour;
	Texture* inkSplatterTex;

	bool isActivated;
	AnimationMultiLerp<float> fadeAnim;

public:
	CgFxInkSplatter(FBObj* sceneFBO);
	virtual ~CgFxInkSplatter();

	virtual void Draw(int screenWidth, int screenHeight, double dT);

	void Activate();
	
	/**
   * Deactivates this effect completely.
	 */
	inline void Deactivate() {
		this->isActivated = false;
	}

	/**
	 * Determines whether the effect is active or not.
	 * Returns: true if active, false otherwise.
	 */
	inline bool IsActive() const {
		return this->isActivated;
	}

};
#endif