/**
 * GameDisplay.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GAMEDISPLAY_H__
#define __GAMEDISPLAY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Camera.h"
#include "DisplayState.h"

#include "../GameModel/GameModel.h"

#include "../GameSound/GameSound.h"

class GameModel;
class GameAssets;
class GameSound;
class GameEventsListener;

// The main display class, used to execute the main rendering loop
// and adjust size, etc.
class GameDisplay {
public:
	static const int MAX_FRAMERATE;							// The maximum framerate possible for the game				
	static const unsigned long FRAME_SLEEP_MS;	// Time to sleep between frames (determined by MAX_FRAMERATE)

	GameDisplay(GameModel* model, int initWidth, int initHeight);
	~GameDisplay();

	bool HasGameExited() const { return this->gameExited; }
	bool ShouldGameReinitialize() const { return this->gameReinitialized; }

	void QuitGame() { this->gameExited = true; }
	void ReinitializeGame() { this->gameReinitialized = true; }

	// Change the current state of the display
	void SetCurrentState(DisplayState* state) {
		assert(state != NULL);
		if (this->currState != NULL) {
			delete this->currState;
		}
		this->currState = state;
	}
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

	static float GetTextScalingFactor();

	// Enumeration of the various actions that can be sent to the GameDisplay
	// to inform it of inputs from the user in a general way
    void ButtonPressed(const GameControl::ActionButton& pressedButton, const GameControl::ActionMagnitude& magnitude = GameControl::FullMagnitude);
	void ButtonReleased(const GameControl::ActionButton& releasedButton);
    void MousePressed(const GameControl::MouseButton& pressedButton);
    void MouseReleased(const GameControl::MouseButton& releasedButton);

    void SpecialDirectionPressed(int x, int y);
    void SpecialDirectionReleased();

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
    // Dialate time if necessary...
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

inline void GameDisplay::AddStateToQueue(const DisplayState::DisplayStateType& type, const DisplayStateInfo& info) {
    stateQueue.push_back(std::make_pair(type, info));
}

#endif