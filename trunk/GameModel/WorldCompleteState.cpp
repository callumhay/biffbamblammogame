/**
 * WorldCompleteState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "WorldCompleteState.h"
#include "BallOnPaddleState.h"
#include "GameCompleteState.h"
#include "LevelStartState.h"
#include "GameModel.h"

WorldCompleteState::WorldCompleteState(GameModel* gm) : GameState(gm), waitingForExternalExit(false) {
}

WorldCompleteState::~WorldCompleteState() {
}

/**
 * Executes only once before changing the state based on what happens after the world ends...
 */
void WorldCompleteState::Tick(double seconds) {
	UNUSED_PARAMETER(seconds);

    if (this->waitingForExternalExit) {
        return;
    }

	GameWorld* currWorld = this->gameModel->GetCurrentWorld();

	// EVENT: World is complete
	GameEventManager::Instance()->ActionWorldCompleted(*currWorld);

	// Need to check for end of the game state (i.e, the world that just completed was the last world in the game)
	if (this->gameModel->IsLastWorld()) {
		// EVENT: Game is complete
		GameEventManager::Instance()->ActionGameCompleted();

		// No more levels or worlds, end of game!!
		this->gameModel->SetNextState(new GameCompleteState(this->gameModel));
	}
	else {
		// Increment the world...
        this->gameModel->IncrementCurrentWorldNum();
		//this->gameModel->SetCurrentWorldAndLevel(this->gameModel->currWorldNum + 1, 0, true);

        // NOTE: We don't go to any further state from here -- we wait until a signal to go to the next state
        // is received from the user
        this->waitingForExternalExit = true;
	}
}