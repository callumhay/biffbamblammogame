/**
 * LevelCompleteState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "LevelCompleteState.h"
#include "WorldCompleteState.h"
#include "BallOnPaddleState.h"
#include "LevelStartState.h"
#include "GameModel.h"
#include "GameProgressIO.h"

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
    currWorld->UpdateLastLevelPassedIndex();

    // EVENT: Level is complete
    GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel);

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