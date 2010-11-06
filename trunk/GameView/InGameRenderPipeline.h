#ifndef __INGAMERENDERPIPELINE_H__
#define __INGAMERENDERPIPELINE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/TextLabel.h"

class FBObj;
class GameDisplay;

class InGameRenderPipeline {
public:
	InGameRenderPipeline(GameDisplay* display);
	~InGameRenderPipeline();
	
	void RenderFrameWithoutHUD(double dT);
	void RenderFrame(double dT);

	// Helper functions - all of these are called by RenderFrame in the order
	// provided below...
	void ApplyInGameCamera(double dT);
	FBObj* RenderBackgroundToFBO(double dT);
	FBObj* RenderForegroundToFBO(FBObj* backgroundFBO, double dT);
	void RenderFinalGather(double dT);
	void RenderHUD(double dT);

private:
	GameDisplay* display;	// Not owned by this object

	// HUD related members
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;
	TextLabel2D scoreLabel;

};

#endif // __INGAMERENDERPIPELINE_H__