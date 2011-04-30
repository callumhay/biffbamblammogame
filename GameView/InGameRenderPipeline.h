/**
 * InGameRenderPipeline.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
    void RenderHUDWithAlpha(double dT, float alpha);

private:
	GameDisplay* display;	// Not owned by this object

	// HUD related members
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;

};

#endif // __INGAMERENDERPIPELINE_H__