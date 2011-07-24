/**
 * CgFxInkSplatter.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	CgFxInkSplatter(FBObj* outputFBO, const std::string& maskTexFilepath);
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