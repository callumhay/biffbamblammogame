/**
 * CgFxInkSplatter.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CGFXINKSPLATTER_H__
#define __CGFXINKSPLATTER_H__

#include "CgFxFullscreenGoo.h"

/**
 * Class for handling the full screen shader effect of the ink block's
 * splatter across the screen.
 */
class CgFxInkSplatter : public CgFxFullscreenGoo {
public:
    CgFxInkSplatter(FBObj* outputFBO, const std::string& maskTexFilepath);
    ~CgFxInkSplatter();

    void ActivateInkSplat();
    inline void DeactivateInkSplat() {
        this->isInkSplatActivated = false;
    }
    inline bool IsInkSplatActive() const {
        return this->isInkSplatActivated;
    }
    inline bool IsInkSplatVisible() const {
        return this->isInkSplatVisible;
    }

    void Draw(int screenWidth, int screenHeight, double dT);

private:
	bool isInkSplatActivated;
    bool isInkSplatVisible;
	AnimationMultiLerp<float> inkSplatFadeAnim;

    DISALLOW_COPY_AND_ASSIGN(CgFxInkSplatter);
};
#endif