#ifndef __CGFXINKSPLATTER_H__
#define __CGFXINKSPLATTER_H__

#include "CgFxFullscreenGoo.h"

/**
 * Class for handling the full screen shader effect of the ink block's
 * splatter across the screen.
 */
class CgFxInkSplatter : public CgFxFullscreenGoo {

private:
	bool isInkSplatActivated;
	AnimationMultiLerp<float> inkSplatFadeAnim;

public:
	CgFxInkSplatter(FBObj* inputFBO, FBObj* outputFBO, const std::string& maskTexFilepath);
	virtual ~CgFxInkSplatter();

	void ActivateInkSplat();
	inline void DeactivateInkSplat() {
		this->isInkSplatActivated = false;
	}
	inline bool IsInkSplatActive() const {
		return this->isInkSplatActivated;
	}

	virtual void Draw(int screenWidth, int screenHeight, double dT);

};
#endif