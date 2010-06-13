/**
 * GameFBOAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMEFBOASSETS_H__
#define __GAMEFBOASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/FBObj.h"

#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"
#include "CgFxFullscreenGoo.h"
#include "CgFxInkSplatter.h"

class GameModel;
class GameItem;
class CgFxPostSmokey;
class CgFxPostUberIntense;

/**
 * Storage class for framebuffer object assets used in the game graphics
 * pipeline. These are used for full screen effects.
 */
class GameFBOAssets {
private:
	// FBO assets for the game pipeline
	FBObj* bgFBO;			
	FBObj* fgAndBgFBO;
	FBObj* postFgAndBgFBO;
	FBObj* initialFSEffectFBO;
	FBObj* finalFSEffectFBO;

	FBObj* tempFBO;	// FBO used for temporary work

	// Post-processing / fullscreen filters and effects used with the FBOs
	CgFxGaussianBlur* fgAndBgBlurEffect;
	CgFxBloom* bloomEffect;
	CgFxAfterImage* afterImageEffect;
	CgFxInkSplatter* inkSplatterEffect;
	CgFxFullscreenGoo* stickyPaddleCamEffect;
	CgFxPostSmokey* smokeyCamEffect;
	CgFxPostUberIntense* uberIntenseCamEffect;

	// Misc. Textures and overlays
	Texture* barrelOverlayTex;	// Texture for overlay of the cannon barrel

	bool drawItemsInLastPass;	// Whether or not items get drawn in the final pass

	enum FBOAnimationType { PoisonAnimationType };
	enum FBOAnimationItem { None };
	std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float> > > fboAnimations;

	/**
	 * Trys to find a particular animation and return it.
	 * Returns: A pointer to the found animation or NULL if not found.
	 */
	inline AnimationMultiLerp<float>* FindFBOAnimation(FBOAnimationType type, FBOAnimationItem item) {
		std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float> > >::iterator iter1 = this->fboAnimations.find(type);
		if (iter1 == this->fboAnimations.end()) {
			return NULL;
		}

		std::map<FBOAnimationItem, AnimationMultiLerp<float> >::iterator iter2 = iter1->second.find(item);
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
	inline FBObj* GetPostFullSceneFBO() { return this->postFgAndBgFBO; }
	inline FBObj* GetInitialFullScreenFBO() { return this->initialFSEffectFBO; }
	inline FBObj* GetFinalFullScreenFBO()	{ return this->finalFSEffectFBO; }

	
	inline bool DrawItemsInLastPass() const { return this->drawItemsInLastPass; }

	inline void RenderFullSceneBlur(int width, int height, double dT) {
		this->fgAndBgBlurEffect->Draw(width, height, dT);
	}

	/**
	 * Render all fullscreen effects that are required before
	 * particles and items are drawn.
	 */
	inline FBObj* RenderInitialFullscreenEffects(int width, int height, double dT) {
		// Do some purdy bloom - this adds a nice contrasty highlighted touch to the entire scene
		this->bloomEffect->Draw(width, height, dT);
		// Add motion blur / afterimage effect 
		this->afterImageEffect->Draw(width, height, dT);

		return this->initialFSEffectFBO;
	}

	/**
	 * Render all fullscreen effects that are required at the very
	 * end of the 3D pipeline.
	 */
	void RenderFinalFullscreenEffects(int width, int height, double dT, const GameModel& gameModel);

	void Tick(double dT);
	void ResizeFBOAssets(int width, int height);

	void ActivateItemEffects(const GameItem& item);
	void DeactivateItemEffects(const GameItem& item);
	
	/**
	 * Activates the ink splatter effect - a full screen effect drawn
	 * at the end of the engine pipeline.
	 */
	void ActivateInkSplatterEffect() {
		this->inkSplatterEffect->ActivateInkSplat();
	}
	/**
	 * Deactivates the ink splatter effect.
	 */
	void DeactivateInkSplatterEffect() {
		this->inkSplatterEffect->DeactivateInkSplat();
	}

};
#endif