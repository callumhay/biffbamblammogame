#ifndef __GAMEFBOASSETS_H__
#define __GAMEFBOASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"

#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"

class FBObj;

/**
 * Storage class for framebuffer object assets used in the game graphics
 * pipeline. These are used for full screen effects.
 */
class GameFBOAssets {
private:
	FBObj* bgFBO;			
	FBObj* fgAndBgFBO;
	FBObj* finalFBO;

	CgFxGaussianBlur* fgAndBgBlurEffect;
	CgFxBloom* bloomEffect;
	CgFxAfterImage* afterImageEffect;

public:
	GameFBOAssets(int displayWidth, int displayHeight);
	~GameFBOAssets();

	inline FBObj* GetBackgroundFBO() { return this->bgFBO; }
	inline FBObj* GetFullSceneFBO() { return this->fgAndBgFBO; }
	
	inline void RenderFullSceneBlur(int width, int height) {
		this->fgAndBgBlurEffect->Draw(width, height);
	}

	inline void RenderBloom(int width, int height) {
		this->bloomEffect->Draw(width, height);
	}

	inline void RenderAfterImage(int width, int height) {
		this->afterImageEffect->Draw(width, height);
	}

	void ResizeFBOAssets(int width, int height);
};
#endif