#include "GameFBOAssets.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/PoisonPaddleItem.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight) : bgFBO(NULL), fgAndBgFBO(NULL), 
initialFSEffectFBO(NULL), finalFSEffectFBO(NULL),
fgAndBgBlurEffect(NULL), bloomEffect(NULL), afterImageEffect(NULL), inkSplatterEffect(NULL), 
drawItemsInLastPass(true) {
	
	// Framebuffer object setup
	this->fgAndBgFBO					= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->bgFBO								= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->initialFSEffectFBO	= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::NoAttachment);
	this->finalFSEffectFBO		= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);

	// Effects setup
	this->fgAndBgBlurEffect	= new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->fgAndBgFBO);
	this->afterImageEffect	= new CgFxAfterImage(this->fgAndBgFBO, this->initialFSEffectFBO);
	this->inkSplatterEffect = new CgFxInkSplatter(this->finalFSEffectFBO);
}

GameFBOAssets::~GameFBOAssets() {
	delete this->bgFBO;	
	this->bgFBO = NULL;
	delete this->fgAndBgFBO;
	this->fgAndBgFBO = NULL;

	delete this->initialFSEffectFBO;
	this->initialFSEffectFBO = NULL;
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO = NULL;

	delete this->fgAndBgBlurEffect;
	this->fgAndBgBlurEffect = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;
	delete this->afterImageEffect;
	this->afterImageEffect = NULL;
	delete this->inkSplatterEffect;
	this->inkSplatterEffect = NULL;
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
	
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO	= new FBObj(width, height, Texture::Nearest, FBObj::NoAttachment);
	delete this->finalFSEffectFBO;
	this->finalFSEffectFBO	= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);

	delete this->fgAndBgBlurEffect;
	delete this->bloomEffect;
	delete this->afterImageEffect;
	delete this->inkSplatterEffect;
	this->fgAndBgBlurEffect = new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->fgAndBgFBO);
	this->afterImageEffect	= new CgFxAfterImage(this->fgAndBgFBO, this->initialFSEffectFBO);
	this->inkSplatterEffect	= new CgFxInkSplatter(this->finalFSEffectFBO);

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