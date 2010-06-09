/**
 * FlashHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __FLASHHUD_H__
#define __FLASHHUD_H__

#include "../BlammoEngine/Animation.h"

class FlashHUD {
public:
	FlashHUD();
	~FlashHUD();

	void Activate(double length, float percentIntensity);
	void Deactivate();

	void Draw(double dT, int displayWidth, int displayHeight);

private:
	bool isActive;														// Whether the HUD is active
	AnimationMultiLerp<float> fadeAnimation;	// Fades the flash HUD in and out when activated

};

#endif // __FLASHHUD_H__