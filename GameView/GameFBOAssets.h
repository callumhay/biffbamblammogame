#ifndef __GAMEFBOASSETS_H__
#define __GAMEFBOASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"

class FBObj;
class GameItem;

/**
 * Storage class for framebuffer object assets used in the game graphics
 * pipeline. These are used for full screen effects.
 */
class GameFBOAssets {
private:
	// FBO assets for the game pipeline
	FBObj* bgFBO;			
	FBObj* fgAndBgFBO;
	FBObj* finalFBO;

	// Post-processing / fullscreen filters and effects used with the FBOs
	CgFxGaussianBlur* fgAndBgBlurEffect;
	CgFxBloom* bloomEffect;
	CgFxAfterImage* afterImageEffect;

	enum FBOAnimationType { PoisonAnimationType };
	enum FBOAnimationItem { None };
	std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float>>> fboAnimations;

	/**
	 * Trys to find a particular animation and return it.
	 * Returns: A pointer to the found animation or NULL if not found.
	 */
	inline AnimationMultiLerp<float>* FindFBOAnimation(FBOAnimationType type, FBOAnimationItem item) {
		std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float>>>::iterator iter1 = this->fboAnimations.find(type);
		if (iter1 == this->fboAnimations.end()) {
			return NULL;
		}

		std::map<FBOAnimationItem, AnimationMultiLerp<float>>::iterator iter2 = iter1->second.find(item);
		if (iter2 == iter1->second.end()) {
			return NULL;
		}

		return &(iter2->second);
	}

public:
	GameFBOAssets(int displayWidth, int displayHeight);
	~GameFBOAssets();

	inline FBObj* GetBackgroundFBO() { return this->bgFBO; }
	inline FBObj* GetFullSceneFBO() { return this->fgAndBgFBO; }
	
	inline void RenderFullSceneBlur(int width, int height, double dT) {
		this->fgAndBgBlurEffect->Draw(width, height, dT);
	}

	inline void RenderBloom(int width, int height, double dT) {
		this->bloomEffect->Draw(width, height, dT);
	}

	inline void RenderAfterImage(int width, int height, double dT) {
		this->afterImageEffect->Draw(width, height, dT);
	}

	void Tick(double dT);
	void ResizeFBOAssets(int width, int height);

	void ActivateItemEffects(const GameItem& item);
	void DeactivateItemEffects(const GameItem& item);
};
#endif