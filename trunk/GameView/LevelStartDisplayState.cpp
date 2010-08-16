
#include "LevelStartDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"

const double LevelStartDisplayState::FADE_IN_TIME = 1.5;

LevelStartDisplayState::LevelStartDisplayState(GameDisplay* display) : 
DisplayState(display), renderPipeline(display) {

	// Setup any animations when starting up a level...
	this->fadeInAnimation.SetLerp(0.0, LevelStartDisplayState::FADE_IN_TIME, 1.0f, 0.0f);
	this->fadeInAnimation.SetRepeat(false);
}

LevelStartDisplayState::~LevelStartDisplayState() {
}

void LevelStartDisplayState::RenderFrame(double dT) {
	const Camera& camera   = this->display->GetCamera();

	// Check to see if we're ready to move on to the next state...
	if (true) {
		this->display->SetCurrentStateAsNextQueuedState();
		return;
	}

	renderPipeline.RenderFrameWithoutHUD(dT);

	// Render the fade-in:
	this->fadeInAnimation.Tick(dT);
	// Draw the fade quad overlay
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
																									 ColourRGBA(1, 1, 1, this->fadeInAnimation.GetInterpolantValue()));
	glPopAttrib();
	

}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void LevelStartDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}
