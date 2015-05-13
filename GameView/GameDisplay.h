/**
 * GameDisplay.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Camera.h"
#include "../GameModel/GameModel.h"
#include "../GameSound/GameSound.h"

#include "DisplayState.h"

class GameModel;
class GameAssets;
class GameSound;
class GameEventsListener;
class MouseRenderer;
class MenuBackgroundRenderer;

// The main display class, used to execute the main rendering loop
// and adjust size, etc.
class GameDisplay {
public:
	static const int MAX_FRAMERATE;             // The maximum framerate possible for the game				
	static const unsigned long FRAME_SLEEP_MS;	// Time to sleep between frames (determined by MAX_FRAMERATE)

	GameDisplay(GameModel* model, GameSound* sound, int initWidth, int initHeight);
	~GameDisplay();

	bool HasGameExited() const { return this->gameExited; }
	bool ShouldGameReinitialize() const { return this->gameReinitialized; }

	void QuitGame() { this->gameExited = true; }
	void ReinitializeGame() { this->gameReinitialized = true; }

	// Change the current state of the display
	void SetCurrentState(DisplayState* state);
    void SetCurrentStateNoDeletePreviousState(DisplayState* state) {
        assert(state != NULL);
        this->currState = state;
    }

	void AddStateToQueue(const DisplayState::DisplayStateType& type, const DisplayStateInfo& info = DisplayStateInfo());
	bool SetCurrentStateAsNextQueuedState();
	DisplayState::DisplayStateType GetCurrentDisplayState() const {
		return this->currState->GetType();
	}

    // Only used for the tutorial
    void SetCachedDifficulty(const GameModel::Difficulty& difficulty) { this->cachedDifficulty = difficulty; }
    GameModel::Difficulty GetCachedDifficulty() { return this->cachedDifficulty; }

	void ChangeDisplaySize(int w, int h);
	void Render(double dT);
    void UpdateModel(double dT);

	// Functions for setting up different types of render options
	static void SetInitialRenderOptions();

	// Getters and Setters
	GameAssets* GetAssets() {
		return this->assets;
	}
	GameModel* GetModel() {
		return this->model;
	}
    GameSound* GetSound() {
        return this->sound;
    }
	Camera& GetCamera() {
		return this->gameCamera;
	}

    MouseRenderer* GetMouse() {
        return this->mouseRenderer;
    }

    MenuBackgroundRenderer* GetMenuBGRenderer() {
        return this->menuBGRenderer;
    }

	static float GetTextScalingFactor();

	// Enumeration of the various actions that can be sent to the GameDisplay
	// to inform it of inputs from the user in a general way
    void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude = GameControl::FullMagnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);
    void MouseMoved(int oglWindowCoordX, int oglWindowCoordY);
    void WindowFocus(bool windowHasFocus);

    void SpecialDirectionPressed(float x, float y);
    void SpecialDirectionReleased();

    static bool IsGameInPlay(const GameModel& gameModel, const DisplayState::DisplayStateType& type) {
        return GameState::IsGameInPlayState(gameModel) && DisplayState::IsGameInPlayDisplayState(type);
    }

#ifdef _DEBUG

	static void ToggleDrawDebugBounds() {
		drawDebugBounds = !drawDebugBounds;
	}
	static bool IsDrawDebugBoundsOn() {
		return drawDebugBounds;
	}
	static void ToggleDrawDebugLightGeometry() {
		drawDebugLightGeometry = !drawDebugLightGeometry;
	}
	static bool IsDrawDebugLightGeometryOn() {
		return drawDebugLightGeometry;
	}
	static void ToggleDetachCamera() {
		detachedCamera = !detachedCamera;
	}
	static bool IsCameraDetached() {
		return detachedCamera;
	}

    void ReloadSound() {
        this->sound->ReloadFromMSF();
    }

#endif

private:
	Camera gameCamera;

    std::list<std::pair<DisplayState::DisplayStateType, DisplayStateInfo> > stateQueue;
	DisplayState* currState;

	GameModel*  model;
    GameSound*  sound;
	GameAssets* assets;

    MouseRenderer* mouseRenderer;
    MenuBackgroundRenderer* menuBGRenderer;

	GameEventsListener* gameListener;

	bool gameReinitialized;		// Whether or not we should reinitialize the whole game (recreate the window, etc.)
	bool gameExited;  				// Whether or not the game has been exited

    GameModel::Difficulty cachedDifficulty; // Bit of a hack - used to cache difficulty for when we need to back it up
                                            // for the tutorial level

	// Functions for Action Listeners
	void SetupActionListeners();
	void RemoveActionListeners();

#ifdef _DEBUG
	static bool drawDebugBounds;
	static bool drawDebugLightGeometry;
	static bool detachedCamera;
	// Debug draw stuff
	void DrawDebugAxes();
	void DrawDebugUnitGrid(bool xy, bool xz, bool zy, int numGridTicks);
#endif

};

inline void GameDisplay::Render(double dT) {
    // Dilate time if necessary...
    if (this->currState->GetType() == DisplayState::InGame || 
        this->currState->GetType() == DisplayState::InGameBossLevel ||
        this->currState->GetType() == DisplayState::InTutorialGame) {
        dT *= this->model->GetTimeDialationFactor();
    }

	// Render the current state
	this->currState->RenderFrame(dT);
	debug_opengl_state();

	// Update the game model
    if (this->currState->AllowsGameModelUpdates()) {
        this->UpdateModel(dT);
    }

    // Update sounds
    this->sound->SetListenerPosition(this->gameCamera);
    this->sound->Tick(dT);
}

inline void GameDisplay::UpdateModel(double dT) {
    this->model->Tick(dT);
    this->model->UpdateState();
}

inline void GameDisplay::ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude) {
	this->currState->ButtonPressed(pressedButton, magnitude);
}

inline void GameDisplay::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	this->currState->ButtonReleased(releasedButton);
}

inline void GameDisplay::MousePressed(const GameControl::MouseButton& pressedButton) {
	this->currState->MousePressed(pressedButton);
}

inline void GameDisplay::MouseReleased(const GameControl::MouseButton& releasedButton) {
	this->currState->MouseReleased(releasedButton);
}

inline void GameDisplay::MouseMoved(int oglWindowCoordX, int oglWindowCoordY) {
    this->currState->MouseMoved(oglWindowCoordX, oglWindowCoordY);
}

inline void GameDisplay::WindowFocus(bool windowHasFocus) {
    this->currState->WindowFocus(windowHasFocus);
}

inline void GameDisplay::AddStateToQueue(const DisplayState::DisplayStateType& type, const DisplayStateInfo& info) {
    stateQueue.push_back(std::make_pair(type, info));
}

#endif