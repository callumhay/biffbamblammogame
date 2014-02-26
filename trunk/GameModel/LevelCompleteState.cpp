/**
 * LevelCompleteState.cpp
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

#include "LevelCompleteState.h"
#include "WorldCompleteState.h"
#include "BallOnPaddleState.h"
#include "LevelStartState.h"
#include "GameModel.h"
#include "GameProgressIO.h"
#include "GameTransformMgr.h"

/**
 * When a level completes the game model needs to be set to the appropriate
 * set of variable states so it can continue on to the next level and clear
 * all the craziness of the previous level's state...
 */
LevelCompleteState::LevelCompleteState(GameModel* gm) : GameState(gm), waitingForExternalExit(false) {
    GameWorld* currWorld = this->gameModel->GetCurrentWorld();
    GameLevel* currLevel = currWorld->GetCurrentLevel();

    // Special case: Boss levels, when completed indicate that they were completed by storing a high-score > 0.
    if (currLevel->GetHasBoss()) {
        currLevel->SetHighScore(std::numeric_limits<long>::max(), false);
    }
    else {
        // Check to see whether the high-score for the level was beaten...
        if (currLevel->GetHighScore() < gm->GetScore()) {
            // Set the new high score for the level and indicate that the high score was beaten
            currLevel->SetHighScore(gm->GetScore(), false);
            currLevel->SetNewHighScore(true);
        }
        else {
            currLevel->SetNewHighScore(false);
        }
    }
    
    int levelPassedBefore = currWorld->GetLastLevelIndexPassed();
    currWorld->UpdateLastLevelPassedIndex();
    int levelPassedAfter = currWorld->GetLastLevelIndexPassed();

    // EVENT: Level is complete
    GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel, levelPassedBefore, levelPassedAfter);

    // Check to see if a new world was just unlocked, if it was, unlock it!
    if (currWorld->GetWorldIndex() != this->gameModel->GetLastWorldIndex() &&
        static_cast<int>(currLevel->GetLevelIndex()) == currWorld->GetLastLevelIndex()) {
        
        GameWorld* nextWorld = this->gameModel->GetWorldByIndex(currWorld->GetWorldIndex()+1);
        nextWorld->SetHasBeenUnlocked(true);
    }

    // Save game progress!
    GameProgressIO::SaveGameProgress(this->gameModel);
}

LevelCompleteState::~LevelCompleteState() {
	// Clear up the model since the level is officially completed
	this->gameModel->ClearStatusUpdatePieces();
	this->gameModel->ClearProjectiles();
	this->gameModel->ClearBeams();
	this->gameModel->ClearLiveItems();
	this->gameModel->ClearActiveTimers();
	this->gameModel->GetTransformInfo()->Reset();
}

/**
 * The tick will only happen once and when it does the state will have changed
 * based on what happens to the game after the level ends.
 */
void LevelCompleteState::Tick(double seconds) {
	UNUSED_PARAMETER(seconds);

    if (this->waitingForExternalExit) {
        return;
    }

	// Clean up all the level-related state stuff in preparation for the next level/world/?
	
	// Reset the game transform manager
	this->gameModel->GetTransformInfo()->Reset();

    // Make sure there's only one ball
    this->gameModel->ClearBallsToOne();

	GameWorld* currWorld = this->gameModel->GetCurrentWorld();

    // Determine the next level to increment to and then load the appropriate
    // level, world or end of game depending on what comes next.
	if (currWorld->IsLastLevel()) {
		// World is complete!!!
		// Go to the world complete state!
		this->gameModel->SetNextState(new WorldCompleteState(this->gameModel));
	}
	else {
        // NOTE: We don't go to any further state from here -- we wait until a signal to go to the next state
        // is received from the user
        this->waitingForExternalExit = true;

        // Place the ball back on the paddle, and let the next level begin!
		//this->gameModel->SetNextState(new LevelStartState(this->gameModel));
	}
}