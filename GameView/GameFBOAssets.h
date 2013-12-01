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
class CgFxBloom;

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

    FBObj* RenderInitialFullscreenEffects(int width, int height, double dT);

	inline void RenderFullSceneBlur(int width, int height, double dT) {
        this->blurEffect->SetInputFBO(this->fgAndBgFBO);
        this->blurEffect->SetBlurType(CgFxGaussianBlur::Kernel3x3);
        this->blurEffect->SetSigma(0.8f);
		this->blurEffect->Draw(width, height, dT);
	}
    inline void RenderBackgroundBlur(int width, int height, double dT) {
        this->blurEffect->SetInputFBO(this->bgFBO);
        this->blurEffect->SetBlurType(CgFxGaussianBlur::Kernel3x3);
        this->blurEffect->SetSigma(1.125f);
        this->blurEffect->Draw(width, height, dT);
    }

	/**
	 * Render all full screen effects that are required at the very
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
    SoundID inkSplatterEventSoundID; // For tracking the last ink splat sound and other sounds that get activated and modified in this object

	// FBO assets for the game pipeline
	FBObj* bgFBO;			
	FBObj* fgAndBgFBO;
	FBObj* finalFSEffectFBO;
    FBObj* colourAndDepthTexFBO;
    FBObj* tempFBO;	// FBO used for temporary work

	// Post-processing / full screen filters and effects used with the FBOs
	CgFxGaussianBlur* blurEffect;
	CgFxInkSplatter* inkSplatterEffect;
	CgFxFullscreenGoo* stickyPaddleCamEffect;
	CgFxPostSmokey* smokeyCamEffect;
	CgFxPostSmokey* icyCamEffect;
	CgFxPostUberIntense* uberIntenseCamEffect;
	CgFxFullscreenGoo* shieldPaddleCamEffect;
	CgFxPostFirey* fireBallCamEffect;
    CgFxPostBulletTime* bulletTimeEffect;
    CgFxBloom* bloomEffect;
    CgFxCelOutlines celOutlineEffect;

	bool drawItemsInLastPass;	// Whether or not items get drawn in the final pass

	enum FBOAnimationType { PoisonAnimationType };
	enum FBOAnimationItem { None };
	std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float> > > fboAnimations;
    
    DISALLOW_COPY_AND_ASSIGN(GameFBOAssets);
};
#endif