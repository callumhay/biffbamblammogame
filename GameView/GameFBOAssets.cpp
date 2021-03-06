/**
 * GameFBOAssets.cpp
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

#include "GameFBOAssets.h"
#include "GameViewConstants.h"
#include "CgFxPostSmokey.h"
#include "CgFxPostUberIntense.h"
#include "CgFxPostFirey.h"
#include "CgFxBloom.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

#include "../GameSound/GameSound.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight, GameSound* sound) :
sound(sound), bgFBO(NULL), fgAndBgFBO(NULL), finalFSEffectFBO(NULL), tempFBO(NULL), colourAndDepthTexFBO(NULL),
blurEffect(NULL), inkSplatterEffect(NULL), bloomEffect(NULL),
stickyPaddleCamEffect(NULL), shieldPaddleCamEffect(NULL), smokeyCamEffect(NULL), icyCamEffect(NULL), 
uberIntenseCamEffect(NULL), fireBallCamEffect(NULL), bulletTimeEffect(NULL), 
drawItemsInLastPass(true), inkSplatterEventSoundID(INVALID_SOUND_ID) {
	
    assert(sound != NULL);

	// Framebuffer object setup
	this->fgAndBgFBO            = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->bgFBO                 = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->finalFSEffectFBO      = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->tempFBO               = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
    this->colourAndDepthTexFBO  = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthTextureAttachment);

	// Effects setup
	this->blurEffect = new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->inkSplatterEffect	= new CgFxInkSplatter(this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);
    this->bloomEffect = new CgFxBloom(this->fgAndBgFBO, this->blurEffect);
	this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);

	this->smokeyCamEffect = new CgFxPostSmokey(this->tempFBO);

	this->icyCamEffect = new CgFxPostSmokey(this->tempFBO);
	this->icyCamEffect->SetTechnique(CgFxPostSmokey::POST_ICY_TECHNIQUE_NAME);
	this->icyCamEffect->SetScale(0.4f);
	this->icyCamEffect->SetFrequency(0.4f);

	this->uberIntenseCamEffect = new CgFxPostUberIntense(this->tempFBO);
	this->fireBallCamEffect    = new CgFxPostFirey(this->tempFBO);
    this->bulletTimeEffect     = new CgFxPostBulletTime(this->tempFBO);

	this->SetupPaddleShieldEffect();
}

GameFBOAssets::~GameFBOAssets() {
	delete this->bgFBO;	
	this->bgFBO = NULL;
	delete this->fgAndBgFBO;
	this->fgAndBgFBO = NULL;

	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO = NULL;

	delete this->tempFBO;
	this->tempFBO = NULL;

    delete this->colourAndDepthTexFBO;
    this->colourAndDepthTexFBO = NULL;

	delete this->blurEffect;
	this->blurEffect = NULL;
	delete this->inkSplatterEffect;
	this->inkSplatterEffect = NULL;
	delete this->stickyPaddleCamEffect;
	this->stickyPaddleCamEffect = NULL;
	delete this->smokeyCamEffect;
	this->smokeyCamEffect = NULL;
	delete this->icyCamEffect;
	this->icyCamEffect = NULL;
	delete this->uberIntenseCamEffect;
	this->uberIntenseCamEffect = NULL;
	delete this->shieldPaddleCamEffect;
	this->shieldPaddleCamEffect = NULL;
	delete this->fireBallCamEffect;
	this->fireBallCamEffect = NULL;
    delete this->bulletTimeEffect;
    this->bulletTimeEffect = NULL;

    delete this->bloomEffect;
    this->bloomEffect = NULL;
}

FBObj* GameFBOAssets::RenderInitialFullscreenEffects(int width, int height, double dT) {
    this->bloomEffect->Draw(width, height, dT);
    return this->fgAndBgFBO;
}

/**
 * Used to animate and do any time-related activities with the FBOs.
 */
void GameFBOAssets::Tick(double dT) {

	for (std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float> > >::iterator iter1 = this->fboAnimations.begin();
		iter1 != this->fboAnimations.end(); ++iter1) {

        std::list<std::map<FBOAnimationItem, AnimationMultiLerp<float> >::iterator > toRemove;
		std::map<FBOAnimationItem, AnimationMultiLerp<float> >& currAnimSet = iter1->second;
		for (std::map<FBOAnimationItem, AnimationMultiLerp<float> >::iterator iter2 = currAnimSet.begin(); iter2 != currAnimSet.end(); ++iter2) {
			bool finished = iter2->second.Tick(dT);
			if (finished) {
				toRemove.push_back(iter2);
			}
		}

        for (std::list<std::map<FBOAnimationItem, AnimationMultiLerp<float> >::iterator >::iterator iterRemove = toRemove.begin();
                iterRemove != toRemove.end(); ++iterRemove) {
            currAnimSet.erase(*iterRemove);
        }
	}
}

/**
 * Used to resize the FBO assets (usually to the screen/window size).
 */
void GameFBOAssets::ResizeFBOAssets(int width, int height) {
	delete this->bgFBO;
	this->bgFBO = new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);
	delete this->fgAndBgFBO;
	this->fgAndBgFBO	= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO	= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);

	delete this->tempFBO;
	this->tempFBO = new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);

    delete this->colourAndDepthTexFBO;
    this->colourAndDepthTexFBO = new FBObj(width, height, Texture::Nearest, FBObj::DepthTextureAttachment);

	delete this->blurEffect;
	delete this->inkSplatterEffect;
	delete this->stickyPaddleCamEffect;
	delete this->smokeyCamEffect;
	delete this->icyCamEffect;
	delete this->uberIntenseCamEffect;
	delete this->shieldPaddleCamEffect;
	delete this->fireBallCamEffect;
    delete this->bulletTimeEffect;

    delete this->bloomEffect;

    this->smokeyCamEffect   = new CgFxPostSmokey(this->tempFBO);
	this->blurEffect = new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->inkSplatterEffect = new CgFxInkSplatter(this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);
	this->bloomEffect       = new CgFxBloom(this->fgAndBgFBO, this->blurEffect);
    this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);
	
	this->icyCamEffect = new CgFxPostSmokey(this->tempFBO);
	this->icyCamEffect->SetTechnique(CgFxPostSmokey::POST_ICY_TECHNIQUE_NAME);
	this->icyCamEffect->SetScale(0.4f);
	this->icyCamEffect->SetFrequency(0.4f);

	this->uberIntenseCamEffect	= new CgFxPostUberIntense(this->tempFBO);
	this->fireBallCamEffect     = new CgFxPostFirey(this->tempFBO);
    this->bulletTimeEffect      = new CgFxPostBulletTime(this->tempFBO);
	this->SetupPaddleShieldEffect();

	debug_opengl_state();
}

/**
 * Used to activate / setup any item-related effects that have to do with
 * any use of the FBO assets.
 */
void GameFBOAssets::ActivateItemEffects(const GameItem& item) {

	switch (item.GetItemType()) {

		case GameItem::PoisonPaddleItem: {
				// Poison items cause the blur to get fuzzier, tell the blur to do this
				this->blurEffect->SetPoisonBlurAnimation(true);
				// Make items also blurry - in order to do this they must be drawn before the post-processing effects
				this->drawItemsInLastPass = false;
			}
			break;

		default:
			break;
	}
}

/**
 * Used to deactivate any item-related effects that have to do with
 * any use of the FBO assets.
 */
void GameFBOAssets::DeactivateItemEffects(const GameItem& item) {

	switch (item.GetItemType()) {

		case GameItem::PoisonPaddleItem: {
				// Turn off the poison blur effect
				this->blurEffect->SetPoisonBlurAnimation(false);
				this->drawItemsInLastPass = true;
			}
			break;

		default:
			break;
	}
}

/**
 * Render all fullscreen effects that are required at the very
 * end of the 3D pipeline.
 */
void GameFBOAssets::RenderFinalFullscreenEffects(int width, int height, double dT, const GameModel& gameModel) {
	
    this->blurEffect->Tick(dT);

	// Use these to ping-pong our final fullscreen effect shader passes
	FBObj* inputFBO		= this->finalFSEffectFBO;
	FBObj* outputFBO	= this->tempFBO;
	FBObj* swapFBO		= NULL;

	if (this->inkSplatterEffect->IsInkSplatActive()) {
 		this->inkSplatterEffect->SetInputFBO(inputFBO);
		this->inkSplatterEffect->SetOutputFBO(outputFBO);
		this->inkSplatterEffect->Draw(width, height, dT);
		swapFBO = inputFBO;
		inputFBO = outputFBO;
		outputFBO = swapFBO;

        // The ink splatter might no longer be active, in this case we turn off any effects that
        // got toggled when it was activated
        if (this->inkSplatterEffect->IsInkSplatVisible()) {
            sound->ToggleSoundEffect(GameSound::InkSplatterEffect, true, this->inkSplatterEventSoundID);
        }
        else {
            sound->ToggleSoundEffect(GameSound::InkSplatterEffect, false, this->inkSplatterEventSoundID);
        }
	}

    // This is a bit of a hack to make sure that the radial blur doesn't cause strange artifacts
    // around the edges of the screen when bullet time mode is activated...
	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	if (paddle->GetIsPaddleCameraOn()) {
		
		if (paddle->HasPaddleType(PlayerPaddle::StickyPaddle)) {

			// We take inverse of the alpha of the paddle to fade the sticky goo effect properly while in paddle cam mode
			float stickyGooAlpha = 1.0f - paddle->GetAlpha();
			this->stickyPaddleCamEffect->SetFadeAlpha(stickyGooAlpha);
			this->stickyPaddleCamEffect->SetInputFBO(inputFBO);
			this->stickyPaddleCamEffect->SetOutputFBO(outputFBO);
			this->stickyPaddleCamEffect->Draw(width, height, dT);

			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}
		
		if (paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {

			float shieldAlpha = 1.0f - paddle->GetAlpha();
			this->shieldPaddleCamEffect->SetFadeAlpha(shieldAlpha);
			this->shieldPaddleCamEffect->SetInputFBO(inputFBO);
			this->shieldPaddleCamEffect->SetOutputFBO(outputFBO);
			this->shieldPaddleCamEffect->Draw(width, height, dT);

			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;

			this->uberIntenseCamEffect->SetFadeAlpha(shieldAlpha);
			this->uberIntenseCamEffect->SetColour(GameModelConstants::GetInstance()->SHIELD_PADDLE_COLOUR);
			this->uberIntenseCamEffect->SetInputFBO(inputFBO);
			this->uberIntenseCamEffect->SetOutputFBO(outputFBO);
			this->uberIntenseCamEffect->Draw(width, height, dT);
			
			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}

        if (paddle->HasSpecialStatus(PlayerPaddle::FrozenInIceStatus)) {
            // The paddle camera is on and the paddle is frozen in ice, make everything frozen/frosty for the player...
            // We take inverse of the alpha of the paddle to fade the effect properly while in paddle cam mode
            float icyAlpha = 1.0f - paddle->GetAlpha();
            this->icyCamEffect->SetFadeAlpha(icyAlpha);
            this->icyCamEffect->SetInputFBO(inputFBO);
            this->icyCamEffect->SetOutputFBO(outputFBO);
            this->icyCamEffect->Draw(width, height, dT);

            swapFBO = inputFBO;
            inputFBO = outputFBO;
            outputFBO = swapFBO;
        }
	}
	else if (GameBall::GetIsBallCameraOn()) {
		const GameBall* camBall = GameBall::GetBallCameraBall();
		assert(camBall != NULL);

		if (gameModel.IsBallEffectActive(GameBall::GhostBall)) {
			// The ball camera is on and we're in ghost ball mode, make everything smokey/foggy for the player...
			// We take inverse of the alpha of the ball to fade the smokey effect properly while in ball cam mode
			float smokeyAlpha = 1.0f - camBall->GetColour().A();
			this->smokeyCamEffect->SetFadeAlpha(smokeyAlpha);
			this->smokeyCamEffect->SetInputFBO(inputFBO);
			this->smokeyCamEffect->SetOutputFBO(outputFBO);
			this->smokeyCamEffect->Draw(width, height, dT);
			
			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}

		if (gameModel.IsBallEffectActive(GameBall::IceBall)) {
			// The ball camera is on and we're in ice ball mode, make everything frozen/frosty for the player...
			// We take inverse of the alpha of the ball to fade the effect properly while in ball cam mode
			float icyAlpha = 1.0f - camBall->GetColour().A();
			this->icyCamEffect->SetFadeAlpha(icyAlpha);
			this->icyCamEffect->SetInputFBO(inputFBO);
			this->icyCamEffect->SetOutputFBO(outputFBO);
			this->icyCamEffect->Draw(width, height, dT);

			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}

		if (gameModel.IsBallEffectActive(GameBall::UberBall)) {
			// The ball camera is on and we're in uber ball mode, make the screen pulse with intensity...
			// We take inverse of the alpha of the ball to fade the smokey effect properly while in ball cam mode
			float uberAlpha = 1.0f - camBall->GetColour().A();
			this->uberIntenseCamEffect->SetFadeAlpha(uberAlpha);
			this->uberIntenseCamEffect->SetColour(GameModelConstants::GetInstance()->UBER_BALL_COLOUR);
			this->uberIntenseCamEffect->SetInputFBO(inputFBO);
			this->uberIntenseCamEffect->SetOutputFBO(outputFBO);
			this->uberIntenseCamEffect->Draw(width, height, dT);
			
			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}
		
		if (gameModel.IsBallEffectActive(GameBall::FireBall)) {
			float fireAlpha = 1.0f - camBall->GetColour().A();
			this->fireBallCamEffect->SetFadeAlpha(fireAlpha);
			this->fireBallCamEffect->SetInputFBO(inputFBO);
			this->fireBallCamEffect->SetOutputFBO(outputFBO);
			this->fireBallCamEffect->Draw(width, height, dT);

			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}
	}
    else {
        // Neither the ball or paddle camera is on - the player could be entering, exiting or 
        // experiencing the ball bullet time for the boost mechanic, update the bullet time effect
        // with the current boost state and possibly draw it
        this->bulletTimeEffect->UpdateAndDraw(dT, gameModel.GetBallBoostModel(), inputFBO, outputFBO);
    }

	assert(inputFBO != NULL);
	assert(outputFBO != NULL);
	assert(outputFBO != inputFBO);

	inputFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(1.0f);

	this->finalFSEffectFBO = inputFBO;
	this->tempFBO = outputFBO;
}

/**
 * Activates the ink splatter effect - a full screen effect drawn
 * at the end of the engine pipeline.
 */
void GameFBOAssets::ActivateInkSplatterEffect() {
    // Activates the ink splat, but there's an animation, so we don't apply the splatter effect just yet
	this->inkSplatterEffect->ActivateInkSplat();
    
    this->inkSplatterEventSoundID = sound->PlaySound(GameSound::InkSplatterEvent, false, true);
}
/**
 * Deactivates the ink splatter effect.
 */
void GameFBOAssets::DeactivateInkSplatterEffect() {
    // Stops the ink splat immediately -- need to kill the sound too
	this->inkSplatterEffect->DeactivateInkSplat();
    sound->ToggleSoundEffect(GameSound::InkSplatterEffect, false, this->inkSplatterEventSoundID);
    this->inkSplatterEventSoundID = INVALID_SOUND_ID;
}

void GameFBOAssets::SetupPaddleShieldEffect() {
	this->shieldPaddleCamEffect	= new CgFxFullscreenGoo(this->tempFBO);
	this->shieldPaddleCamEffect->SetColour(Colour(1, 1, 1));
	this->shieldPaddleCamEffect->SetDisplacement(0.02f);
	this->shieldPaddleCamEffect->SetScale(1.5f);
}