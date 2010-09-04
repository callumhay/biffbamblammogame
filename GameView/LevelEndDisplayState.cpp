
#include "LevelEndDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"

#include "../GameModel/GameModel.h"
#include "../BlammoEngine/Camera.h"

const double LevelEndDisplayState::FADE_TIME = 2.0;

LevelEndDisplayState::LevelEndDisplayState(GameDisplay* display) : DisplayState(display),
renderPipeline(display) {

	// Pause all game play elements in the game model
	this->display->GetModel()->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

	this->fadeToWhiteAnimation.SetLerp(0.0, LevelEndDisplayState::FADE_TIME, 0.0f, 1.0f);
	this->fadeToWhiteAnimation.SetRepeat(false);
	this->fadeToWhiteAnimation.SetInterpolantValue(0.0f);

}

LevelEndDisplayState::~LevelEndDisplayState() {
}

void LevelEndDisplayState::RenderFrame(double dT) {
	const Camera& camera = this->display->GetCamera();

	// Clear the screen to a white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw a fullscreen quad of the last fully rendered frame from the game...
	const Texture2D* lastSceneTexture = this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()->GetFBOTexture();
	lastSceneTexture->RenderTextureToFullscreenQuad();
	this->renderPipeline.RenderHUD(dT);

	bool fadeIsDone = this->fadeToWhiteAnimation.Tick(dT);

	// Apply the fade out to signify the end of the level...
	// Draw the fade quad overlay
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(camera.GetWindowWidth(), camera.GetWindowHeight(), 1.0f, 
																									 ColourRGBA(1, 1, 1, this->fadeToWhiteAnimation.GetInterpolantValue()));

	glPopAttrib();

	if (fadeIsDone) {
		this->display->SetCurrentStateAsNextQueuedState();
		return;
	}
}
