#include "GameFBOAssets.h"
#include "GameViewConstants.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/PoisonPaddleItem.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight) : bgFBO(NULL), fgAndBgFBO(NULL), 
postFgAndBgFBO(NULL), initialFSEffectFBO(NULL), finalFSEffectFBO(NULL), tempFBO(NULL),
fgAndBgBlurEffect(NULL), bloomEffect(NULL), afterImageEffect(NULL), inkSplatterEffect(NULL), stickyPaddleCamEffect(NULL),
drawItemsInLastPass(true) {
	
	// Framebuffer object setup
	this->fgAndBgFBO					= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->bgFBO								= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->postFgAndBgFBO			= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->initialFSEffectFBO	= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->finalFSEffectFBO		= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);

	this->tempFBO = new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);

	// Effects setup
	this->fgAndBgBlurEffect	= new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->postFgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->postFgAndBgFBO);
	this->afterImageEffect	= new CgFxAfterImage(this->postFgAndBgFBO, this->initialFSEffectFBO);

	this->inkSplatterEffect	= new CgFxInkSplatter(this->finalFSEffectFBO, this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);

	this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->finalFSEffectFBO, this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);
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
}

/**
 * Used to animate and do any time-related activites with the FBOs.
 */
void GameFBOAssets::Tick(double dT) {
	for (std::map<FBOAnimationType, std::map<FBOAnimationItem, AnimationMultiLerp<float>>>::iterator iter1 = this->fboAnimations.begin();
		iter1 != this->fboAnimations.end(); iter1++) {
		
		std::map<FBOAnimationItem, AnimationMultiLerp<float>>& currAnimSet = iter1->second;
		for (std::map<FBOAnimationItem, AnimationMultiLerp<float>>::iterator iter2 = currAnimSet.begin(); iter2 != currAnimSet.end();) {
			bool finished = iter2->second.Tick(dT);
			if (finished) {
				iter2 = currAnimSet.erase(iter2);
			}
			else {
				iter2++;
			}
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

	this->fgAndBgBlurEffect = new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->postFgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->postFgAndBgFBO);
	this->afterImageEffect	= new CgFxAfterImage(this->postFgAndBgFBO, this->initialFSEffectFBO);
	
	this->inkSplatterEffect	= new CgFxInkSplatter(this->finalFSEffectFBO, this->tempFBO, GameViewConstants::GetInstance()->TEXTURE_INKSPLATTER);

	this->stickyPaddleCamEffect = new CgFxFullscreenGoo(this->finalFSEffectFBO, this->tempFBO);
	this->stickyPaddleCamEffect->SetColour(GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR);

	debug_opengl_state();
}

/**
 * Used to activate / setup any item-related effects that have to do with
 * any use of the FBO assets.
 */
void GameFBOAssets::ActivateItemEffects(const GameItem& item) {

	if (item.GetName() == PoisonPaddleItem::POISON_PADDLE_ITEM_NAME) {
		// Poison items cause the blur to get fuzzier, tell the blur to do this
		this->fgAndBgBlurEffect->SetPoisonBlurAnimation(true);
		this->afterImageEffect->SetBlurAmount(0.4f);
		// Make items also blurry - in order to do this they must be drawn before the post-processing effects
		this->drawItemsInLastPass = false;
	}

}

/**
 * Used to deactivate any item-related effects that have to do with
 * any use of the FBO assets.
 */
void GameFBOAssets::DeactivateItemEffects(const GameItem& item) {

	if (item.GetName() == PoisonPaddleItem::POISON_PADDLE_ITEM_NAME) {
		// Turn off the poison blur effect
		this->fgAndBgBlurEffect->SetPoisonBlurAnimation(false);
		this->afterImageEffect->SetBlurAmount(CgFxAfterImage::AFTERIMAGE_BLURSTRENGTH_DEFAULT);
		this->drawItemsInLastPass = true;
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
		// We take inverse of the alpha of the paddle to fade the sticky goo effect properly while in paddle cam
		float stickyGooAlpha = 1.0f - paddle->GetPaddleColour().A();

		this->stickyPaddleCamEffect->SetFadeAlpha(stickyGooAlpha);
		this->stickyPaddleCamEffect->SetInputFBO(inputFBO);
		this->stickyPaddleCamEffect->SetOutputFBO(outputFBO);
		this->stickyPaddleCamEffect->Draw(width, height, dT);

		swapFBO = inputFBO;
		inputFBO = outputFBO;
		outputFBO = swapFBO;
	}

	inputFBO->GetFBOTexture()->RenderTextureToFullscreenQuad(-1.0f);
}