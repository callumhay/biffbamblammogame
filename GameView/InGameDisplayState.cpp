/**
 * InGameDisplayState.cpp
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

#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "LivesLeftHUD.h"
#include "CgFxGaussianBlur.h"

#include "../GameModel/GameModel.h"
#include "../GameModel/Beam.h"
#include "../WindowManager.h"

InGameDisplayState::InGameDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display) {
	
	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
    this->display->GetModel()->SetPauseState(GameModel::NoPause);
	this->display->GetCamera().ClearCameraShake();

	debug_opengl_state();
}

InGameDisplayState::~InGameDisplayState() {
    if (!WindowManager::GetInstance()->GetIsFullscreen()) {
        WindowManager::GetInstance()->ShowCursor(true);
    }
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

void InGameDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                       const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);

	// We only interpret one key press - when the user wants to access the in-game menu...
	if (this->IsInGameMenuEnabled() &&
       (pressedButton == GameControl::EscapeButtonAction || pressedButton == GameControl::PauseButtonAction)) {
		// Go to the next state
		this->display->SetCurrentStateNoDeletePreviousState(new InGameMenuState(this->display, this));
	}
}

/**
 * Called when the display size changes - we need to recreate our framebuffer objects 
 * to account for the new screen size.
 */
void InGameDisplayState::DisplaySizeChanged(int width, int height) {
	this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}