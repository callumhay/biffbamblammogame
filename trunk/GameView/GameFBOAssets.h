/**
 * GameFBOAssets.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    inline void RenderBlur(int width, int height, FBObj* blurBuffer, float sigma) {
        this->blurEffect->SetInputFBO(blurBuffer);
        this->blurEffect->SetBlurType(CgFxGaussianBlur::Kernel3x3);
        this->blurEffect->SetSigma(sigma);
        this->blurEffect->Draw(width, height);
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