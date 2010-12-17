
#include "DisplayState.h"
#include "MainMenuDisplayState.h"
#include "BlammopediaState.h"
#include "LevelStartDisplayState.h"
#include "WorldStartDisplayState.h"
#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "LevelEndDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "GameOverDisplayState.h"

// Factory method for building a display state from a given enumerated
// type. Caller takes ownership of returned memory.
DisplayState* DisplayState::BuildDisplayStateFromType(const DisplayStateType& type, GameDisplay* display) {
	switch (type) {
		case DisplayState::MainMenu:
			return new MainMenuDisplayState(display);
        case DisplayState::BlammopediaMenu:
            return new BlammopediaState(display);
		case DisplayState::LevelStart:
			return new LevelStartDisplayState(display);
		case DisplayState::WorldStart:
			return new WorldStartDisplayState(display);
		case DisplayState::InGame:
			return new InGameDisplayState(display);
		case DisplayState::InGameMenu:
			return new InGameMenuState(display);
		case DisplayState::LevelEnd:
			return new LevelEndDisplayState(display);
		case DisplayState::GameComplete:
			return new GameCompleteDisplayState(display);
		case DisplayState::GameOver:
			return new GameOverDisplayState(display);
		default:
			assert(false);
			break;
	}

	return NULL;
}