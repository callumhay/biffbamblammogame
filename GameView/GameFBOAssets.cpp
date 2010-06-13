/**
 * GameFBOAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GameFBOAssets.h"
#include "GameViewConstants.h"
#include "CgFxPostSmokey.h"
#include "CgFxPostUberIntense.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight) : bgFBO(NULL), fgAndBgFBO(NULL), 
postFgAndBgFBO(NULL), initialFSEffectFBO(NULL), finalFSEffectFBO(NULL), tempFBO(NULL),
fgAndBgBlurEffect(NULL), bloomEffect(NULL), afterImageEffect(NULL), inkSplatterEffect(NULL), 
stickyPaddleCamEffect(NULL), smokeyCamEffect(NULL), uberIntenseCamEffect(NULL),
drawItemsInLastPass(true) {
	
	// Framebuffer object setup
	this->fgAndBgFBO					= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->bgFBO								= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->postFgAndBgFBO			= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->initialFSEffectFBO	= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->finalFSEffectFBO		= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);

	this->tempFBO = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);

	// Effects setup
	this->fgAndBgBlurEffect			= new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->postFgAndBgFBO);
	this->bloomEffect						= new CgFxBloom(this->postFgAndBgFBO);
	this->afterImageEffect			= new CgFxAfterImage(this->postFgAndBgFBO, this->initialFSEffectFBO);
	this->inkSplatterEffect			= new CgFxInkSplatter(this->finalFSEffectFBO, this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);
	this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->finalFSEffectFBO, this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);
	this->smokeyCamEffect				= new CgFxPostSmokey(this->finalFSEffectFBO, this->tempFBO);
	this->uberIntenseCamEffect	= new CgFxPostUberIntense(this->finalFSEffectFBO, this->tempFBO);


	this->barrelOverlayTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BARREL_OVERLAY, Texture::Trilinear);
	assert(this->barrelOverlayTex != NULL);
}

GameFBOAssets::~GameFBOAssets() {
	delete this->bgFBO;	
	this->bgFBO = NULL;
	delete this->fgAndBgFBO;
	this->fgAndBgFBO = NULL;
	delete this->postFgAndBgFBO;
	this->postFgAndBgFBO = NULL;

	delete this->initialFSEffectFBO;
	this->initialFSEffectFBO = NULL;
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO = NULL;

	delete this->tempFBO;
	this->tempFBO = NULL;

	delete this->fgAndBgBlurEffect;
	this->fgAndBgBlurEffect = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;
	delete this->afterImageEffect;
	this->afterImageEffect = NULL;
	delete this->inkSplatterEffect;
	this->inkSplatterEffect = NULL;
	delete this->stickyPaddleCamEffect;
	this->stickyPaddleCamEffect = NULL;
	delete this->smokeyCamEffect;
	this->smokeyCamEffect = NULL;
	delete this->uberIntenseCamEffect;
	this->uberIntenseCamEffect = NULL;

	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->barrelOverlayTex);
	assert(success);
}

/**
 * Used to animate and do any time-related activites with the FBOs.
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
	delete this->postFgAndBgFBO;
	this->postFgAndBgFBO = new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);
	
	delete this->initialFSEffectFBO;
	this->initialFSEffectFBO	= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO	= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);

	delete this->tempFBO;
	this->tempFBO = new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);

	delete this->fgAndBgBlurEffect;
	delete this->bloomEffect;
	delete this->afterImageEffect;
	delete this->inkSplatterEffect;
	delete this->stickyPaddleCamEffect;
	delete this->smokeyCamEffect;
	delete this->uberIntenseCamEffect;

	this->fgAndBgBlurEffect			= new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->postFgAndBgFBO);
	this->bloomEffect						= new CgFxBloom(this->postFgAndBgFBO);
	this->afterImageEffect			= new CgFxAfterImage(this->postFgAndBgFBO, this->initialFSEffectFBO);
	this->inkSplatterEffect			= new CgFxInkSplatter(this->finalFSEffectFBO, this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);
	this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->finalFSEffectFBO, this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);
	this->smokeyCamEffect				= new CgFxPostSmokey(this->finalFSEffectFBO, this->tempFBO);
	this->uberIntenseCamEffect	= new CgFxPostUberIntense(this->finalFSEffectFBO, this->tempFBO);

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
				this->fgAndBgBlurEffect->SetPoisonBlurAnimation(true);
				this->afterImageEffect->SetBlurAmount(0.4f);
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
				this->fgAndBgBlurEffect->SetPoisonBlurAnimation(false);
				this->afterImageEffect->SetBlurAmount(CgFxAfterImage::AFTERIMAGE_BLURSTRENGTH_DEFAULT);
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
	}

	const PlayerPaddle* paddle = gameModel.GetPlayerPaddle();
	if (paddle->GetIsPaddleCameraOn() && (paddle->GetPaddleType() & PlayerPaddle::StickyPaddle) == PlayerPaddle::StickyPaddle) {
		// We take inverse of the alpha of the paddle to fade the sticky goo effect properly while in paddle cam mode
		float stickyGooAlpha = 1.0f - paddle->GetColour().A();
		this->stickyPaddleCamEffect->SetFadeAlpha(stickyGooAlpha);
		this->stickyPaddleCamEffect->SetInputFBO(inputFBO);
		this->stickyPaddleCamEffect->SetOutputFBO(outputFBO);
		this->stickyPaddleCamEffect->Draw(width, height, dT);

		swapFBO = inputFBO;
		inputFBO = outputFBO;
		outputFBO = swapFBO;
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

		if (gameModel.IsBallEffectActive(GameBall::UberBall)) {
			// The ball camera is on and we're in uber ball mode, make the screen pulse with intensity...
			// We take inverse of the alpha of the ball to fade the smokey effect properly while in ball cam mode
			float uberAlpha = 1.0f - camBall->GetColour().A();
			this->uberIntenseCamEffect->SetFadeAlpha(uberAlpha);
			this->uberIntenseCamEffect->SetInputFBO(inputFBO);
			this->uberIntenseCamEffect->SetOutputFBO(outputFBO);
			this->uberIntenseCamEffect->Draw(width, height, dT);
			
			swapFBO = inputFBO;
			inputFBO = outputFBO;
			outputFBO = swapFBO;
		}
			
	}

	assert(inputFBO != NULL);
	assert(outputFBO != NULL);
	assert(outputFBO != inputFBO);
	inputFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);

	if (GameBall::GetIsBallCameraOn()) {
		const GameBall* camBall = GameBall::GetBallCameraBall();
		assert(camBall != NULL);
		if (camBall->IsLoadedInCannonBlock()) {
			float overlayAlpha = std::min<float>(0.7f, 1.0f - camBall->GetColour().A());
			assert(overlayAlpha >= 0.0f);

			// If the ball is inside a cannon and is in ball camera mode then we do the James-Bond-esque
			// barrel overlay on the screen...
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			this->barrelOverlayTex->BindTexture();
			GeometryMaker::GetInstance()->DrawFullScreenQuad(width, height, 0.9f, ColourRGBA(1, 1, 1, overlayAlpha));
			this->barrelOverlayTex->UnbindTexture();

			glPopAttrib();
		}
	}

	this->finalFSEffectFBO = inputFBO;
	this->tempFBO = outputFBO;
}