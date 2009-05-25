#include "GameFBOAssets.h"

#include "../BlammoEngine/FBObj.h"

GameFBOAssets::GameFBOAssets(int displayWidth, int displayHeight) : bgFBO(NULL), fgAndBgFBO(NULL), finalFBO(NULL),
fgAndBgBlurEffect(NULL), bloomEffect(NULL), afterImageEffect(NULL) {
	
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