/**
 * LevelEndDisplayState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "LevelEndDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameESPAssets.h"
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
    GameSound* sound = this->display->GetSound();
    sound->StopAllSounds(LevelEndDisplayState::FADE_TIME);
    // Play end-of-level fade out
    sound->PlaySound(GameSound::LevelEndFadeoutEvent, false, true);
    
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw a full-screen quad of the last fully rendered frame from the game...
	const Texture2D* lastSceneTexture = this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()->GetFBOTexture();
	lastSceneTexture->RenderTextureToFullscreenQuad();

	this->renderPipeline.RenderHUD(dT);

	bool fadeIsDone = this->fadeToWhiteAnimation.Tick(dT);

	// Apply the fade out to signify the end of the level...
	// Draw the fade quad overlay
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(Camera::GetWindowWidth(), Camera::GetWindowHeight(), 1.0f, 
        ColourRGBA(1, 1, 1, this->fadeToWhiteAnimation.GetInterpolantValue()));

	glPopAttrib();

	if (fadeIsDone) {
        // Clean up remaining sound stuff
        GameSound* sound = this->display->GetSound();
        sound->StopAllEffects();

	    // Kill all effects that may have previously been occurring...
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

