/**
 * InGameRenderPipeline.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
	virtual ~InGameRenderPipeline();
	
	void RenderFrameWithoutHUD(double dT);
	void RenderFrame(double dT);
    void SetupRenderFrame(double dT);

	// Helper functions - all of these are called by RenderFrame in the order
	// provided below...
	void ApplyInGameCamera(double dT);
	FBObj* RenderBackgroundToFBO(const Vector2D& negHalfLevelDim, double dT);
	FBObj* RenderForegroundToFBO(const Vector2D& negHalfLevelDim, const Matrix4x4& gameTransform, FBObj* backgroundFBO, double dT);
	void RenderFinalGather(const Vector2D& negHalfLevelDim, const Matrix4x4& gameTransform, double dT);
	void RenderHUD(double dT);
    void RenderHUDWithAlpha(double dT, float alpha);

    void SetHUDAlpha(float alpha);

protected:
	GameDisplay* display;	// Not owned by this object

	// HUD related members
	static const unsigned int HUD_X_INDENT;
	static const unsigned int HUD_Y_INDENT;

    virtual void DrawSpecialEmbeddedLabels(double dT) { UNUSED_PARAMETER(dT); };
    virtual void SetSpecialLabelsAlpha(float alpha) { UNUSED_PARAMETER(alpha); };

};

#endif // __INGAMERENDERPIPELINE_H__