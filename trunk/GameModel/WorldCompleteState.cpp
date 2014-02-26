/**
 * WorldCompleteState.cpp
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