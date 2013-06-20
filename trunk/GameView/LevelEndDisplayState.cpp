/**
 * LevelEndDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "LevelEndDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"
#include "PointsHUD.h"
#include "LivesLeftHUD.h"
#include "BallBoostHUD.h"

#include "../GameModel/GameModel.h"
#include "../BlammoEngine/Camera.h"

const double LevelEndDisplayState::RENDER_A_BIT_MORE_TIME = 0.15;
const double LevelEndDisplayState::FADE_TIME = 2.0;

LevelEndDisplayState::LevelEndDisplayState(GameDisplay* display) : DisplayState(display),
renderPipeline(display), renderABitMoreCount(0.0) {

    // Stop all sounds...
    this->display->GetSound()->StopAllSoundLoops();
    // TODO: Play end-of-level fade out sounds/music or something
    

	// Pause all game play elements in the game model
	this->display->GetModel()->SetPauseState(GameModel::PausePaddle | GameModel::PauseBall);

	this->fadeToWhiteAnimation.SetLerp(0.0, LevelEndDisplayState::FADE_TIME, 0.0f, 1.0f);
	this->fadeToWhiteAnimation.SetRepeat(false);
	this->fadeToWhiteAnimation.SetInterpolantValue(0.0f);
}

LevelEndDisplayState::~LevelEndDisplayState() {
}

void LevelEndDisplayState::RenderFrame(double dT) {

    // We keep rendering the game for a tiny bit of time so that the player can
    // see the destruction of the final block(s) that ended the level
    if (this->renderABitMoreCount <= LevelEndDisplayState::RENDER_A_BIT_MORE_TIME) {
        this->renderPipeline.RenderFrame(dT);
#ifdef _DEBUG
	    this->DebugDrawBounds();
	    this->display->GetAssets()->DebugDrawLights();
#endif
        this->renderABitMoreCount += dT;
        return;
    }

	const Camera& camera = this->display->GetCamera();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	    // Kill all effects that may have previously been occuring...
	    this->display->GetAssets()->DeactivateMiscEffects();
        // Kill all particles...
        this->display->GetAssets()->GetESPAssets()->RemoveAllProjectileEffects();

        // Update the game model until there's a new queued state
        while (!this->display->SetCurrentStateAsNextQueuedState()) {
            this->display->UpdateModel(dT);
        }
		return;
	}
}

