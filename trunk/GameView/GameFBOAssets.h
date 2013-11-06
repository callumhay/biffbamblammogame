/**
 * GameFBOAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEFBOASSETS_H__
#define __GAMEFBOASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/FBObj.h"

#include "../GameModel/BallBoostModel.h"

#include "../GameSound/SoundCommon.h"

#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"
#include "CgFxFullscreenGoo.h"
#include "CgFxInkSplatter.h"
#include "CgFxPostBulletTime.h"
#include "CgFxCelOutlines.h"

class GameModel;
class GameSound;
class GameItem;
class CgFxPostSmokey;
class CgFxPostUberIntense;
class CgFxPostFirey;

/**
 * Storage class for framebuffer object assets used in the game graphics
 * pipeline. These are used for full screen effects.
 */
class GameFBOAssets {
public:
	GameFBOAssets(int displayWidth, int displayHeight, GameSound* sound);
	~GameFBOAssets();

	inline FBObj* GetBackgroundFBO() { return this->bgFBO; }
	inline FBObj* GetFullSceneFBO() { return this->fgAndBgFBO; }
	inline FBObj* GetFinalFullScreenFBO()	{ return this->finalFSEffectFBO; }
    inline FBObj* GetColourAndDepthTexFBO() { return this->colourAndDepthTexFBO; }
    inline FBObj* GetTempFBO() { return this->tempFBO; }

    inline CgFxCelOutlines& GetCelOutlineEffect() { return this->celOutlineEffect; }

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
        return this->fgAndBgFBO;
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
	
	void ActivateInkSplatterEffect();
	void DeactivateInkSplatterEffect();

	void SetupPaddleShieldEffect();
    void UpdateBulletTimeState(const BallBoostModel& boostModel) {
        this->bulletTimeEffect->UpdateBulletTimeState(boostModel);
    }

private:
    GameSound* sound; // Not owned by this
    // Sound IDs -- used for tracking the last ink splat sound and other sounds that get activated and modified
    // in this object
    SoundID inkSplatterEventSoundID;


	// FBO assets for the game pipeline
	FBObj* bgFBO;			
	FBObj* fgAndBgFBO;
	FBObj* finalFSEffectFBO;

	FBObj* tempFBO;	// FBO used for temporary work

    FBObj* colourAndDepthTexFBO;

	// Post-processing / fullscreen filters and effects used with the FBOs
	CgFxGaussianBlur* fgAndBgBlurEffect;
	CgFxBloom* bloomEffect;
	CgFxInkSplatter* inkSplatterEffect;
	CgFxFullscreenGoo* stickyPaddleCamEffect;
	CgFxPostSmokey* smokeyCamEffect;
	CgFxPostSmokey* icyCamEffect;
	CgFxPostUberIntense* uberIntenseCamEffect;
	CgFxFullscreenGoo* shieldPaddleCamEffect;
	CgFxPostFirey* fireBallCamEffect;
    CgFxPostBulletTime* bulletTimeEffect;

    CgFxCelOutlines celOutlineEffect;

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

	void DrawCannonBarrelOverlay(int width, int height, float alpha);
    
    DISALLOW_COPY_AND_ASSIGN(GameFBOAssets);
};
#endif