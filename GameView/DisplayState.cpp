/**
 * DisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "DisplayState.h"
#include "MainMenuDisplayState.h"
#include "SelectWorldMenuState.h"
#include "SelectLevelMenuState.h"
#include "BlammopediaState.h"
#include "LevelStartDisplayState.h"
#include "WorldStartDisplayState.h"
#include "InGameDisplayState.h"
#include "InGameMenuState.h"
#include "LevelEndDisplayState.h"
#include "LevelCompleteSummaryDisplayState.h"
#include "GameCompleteDisplayState.h"
#include "GameOverDisplayState.h"

// Factory method for building a display state from a given enumerated
// type. Caller takes ownership of returned memory.
DisplayState* DisplayState::BuildDisplayStateFromType(const DisplayStateType& type, GameDisplay* display) {
	switch (type) {
		case DisplayState::MainMenu:
			return new MainMenuDisplayState(display);
        case DisplayState::SelectWorldMenu:
            return new SelectWorldMenuState(display);

        case DisplayState::SelectLevelMenu:
            assert(false);
            return NULL;

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
        case DisplayState::LevelCompleteSummary:
            return new LevelCompleteSummaryDisplayState(display);
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