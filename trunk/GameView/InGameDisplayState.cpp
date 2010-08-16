#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"
#include "CgFxGaussianBlur.h"
#include "CgFxAfterImage.h"
#include "CgFxBloom.h"

// Game Model stuff
#include "../GameModel/GameModel.h"
#include "../GameModel/Beam.h"



InGameDisplayState::InGameDisplayState(GameDisplay* display) : DisplayState(display), renderPipeline(display) {
	this->display->GetModel()->SetPauseState(GameModel::NoPause);

	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
	this->display->GetCamera().ClearCameraShake();
	// ...

	// Render the first frame - just to make sure everything is loaded
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	this->RenderFrame(0.0);
	glPopAttrib();
	this->display->GetAssets()->GetSoundAssets()->PlayWorldSound(GameSoundAssets::WorldBackgroundMusic);

	debug_opengl_state();
}

InGameDisplayState::~InGameDisplayState() {
}

/**
 * This will render a frame of the actual game while it is in-play.
 */
void InGameDisplayState::RenderFrame(double dT) {
	this->renderPipeline.RenderFrame(dT);

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif

}

void InGameDisplayState::KeyPressed(SDLKey key, SDLMod modifier) {
	// We only interpret one key press - when the user wants to access the in-game menu...
	if (key == SDLK_ESCAPE) {
		// Go to the next state
		this->display->SetCurrentState(new InGameMenuState(this->display));
	}
}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void InGameDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}

#ifdef _DEBUG
/**
 * Debugging function that draws the collision boundries of level pieces.
 */
void InGameDisplayState::DebugDrawBounds() {
	if (!GameDisplay::IsDrawDebugBoundsOn()) { return; }
	Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();

	glPushMatrix();
	glTranslatef(negHalfLevelDim[0], negHalfLevelDim[1], 0.0f);

	// Draw boundry of paddle...
	this->display->GetModel()->GetPlayerPaddle()->DebugDraw();
	
	// Draw of boundries of each block...
	std::vector<std::vector<LevelPiece*>> pieces = this->display->GetModel()->GetCurrentLevel()->GetCurrentLevelLayout();
	for (size_t i = 0; i < pieces.size(); i++) {
		std::vector<LevelPiece*> setOfPieces = pieces[i];
		for (size_t j = 0; j < setOfPieces.size(); j++) {
			setOfPieces[j]->DebugDraw();
		}
	}

	// Draw any beam rays...
	std::list<Beam*>& beams = this->display->GetModel()->GetActiveBeams();
	for (std::list<Beam*>::const_iterator iter = beams.begin(); iter != beams.end(); ++iter) {
		(*iter)->DebugDraw();
	}

	glPopMatrix();
	debug_opengl_state();
}
#endif