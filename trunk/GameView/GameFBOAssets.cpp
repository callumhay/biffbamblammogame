#include "GameFBOAssets.h"

#include "../BlammoEngine/FBObj.h"

#include "../GameModel/GameItem.h"
#include "../GameModel/PoisonPaddleItem.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight) : bgFBO(NULL), fgAndBgFBO(NULL), finalFBO(NULL),
fgAndBgBlurEffect(NULL), bloomEffect(NULL), afterImageEffect(NULL), drawItemsInLastPass(true) {
	
	// Framebuffer object setup
	this->fgAndBgFBO		= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->bgFBO					= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment);
	this->finalFBO			= new FBObj(displayWidth, displayHeight, Texture::Nearest, FBObj::DepthAttachment); // TODO: no attachment...

	// Effects setup
	this->fgAndBgBlurEffect	= new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->fgAndBgFBO);
	this->afterImageEffect	= new CgFxAfterImage(this->fgAndBgFBO);
}

GameFBOAssets::~GameFBOAssets() {
	delete bgFBO;	
	bgFBO = NULL;
	delete fgAndBgFBO;
	fgAndBgFBO = NULL;
	delete finalFBO;
	finalFBO = NULL;

	delete fgAndBgBlurEffect;
	fgAndBgBlurEffect = NULL;
	delete bloomEffect;
	bloomEffect = NULL;
	delete afterImageEffect;
	afterImageEffect = NULL;
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
	delete this->finalFBO;
	this->finalFBO			= new FBObj(width, height, Texture::Nearest, FBObj::DepthAttachment);	// TODO: no attachment...

	delete this->fgAndBgBlurEffect;
	delete this->bloomEffect;
	delete this->afterImageEffect;
	this->fgAndBgBlurEffect = new CgFxGaussianBlur(CgFxGaussianBlur::Kernel3x3, this->fgAndBgFBO);
	this->bloomEffect				= new CgFxBloom(this->fgAndBgFBO);
	this->afterImageEffect = new CgFxAfterImage(this->fgAndBgFBO);

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