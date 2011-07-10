/**
 * LevelCompleteState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelCompleteState.h"
#include "WorldCompleteState.h"
#include "BallOnPaddleState.h"
#include "LevelStartState.h"
#include "GameModel.h"

/**
 * When a level completes the game model needs to be set to the appropriate
 * set of variable states so it can continue on to the next level and clear
 * all the craziness of the previous level's state...
 */
LevelCompleteState::LevelCompleteState(GameModel* gm) : GameState(gm) {
    GameWorld* currWorld = this->gameModel->GetCurrentWorld();
    GameLevel* currLevel = currWorld->GetCurrentLevel();

    // Check to see whether the highscore for the level was beaten...
    if (currLevel->GetHighScore() < gm->GetScore()) {
        // Set the new high score for the level and indicate that the high score was beaten
        currLevel->SetHighScore(gm->GetScore());
        currLevel->SetNewHighScore(true);
    }
    else {
        currLevel->SetNewHighScore(false);
    }

    // EVENT: Level is complete
    GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel);
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

    /*
    if (!firstTickDone) {
	    GameWorld* currWorld = this->gameModel->GetCurrentWorld();
	    GameLevel* currLevel = currWorld->GetCurrentLevel();

	    // EVENT: Level is complete
	    GameEventManager::Instance()->ActionLevelCompleted(*currWorld, *currLevel);
        firstTickDone = true;
    }
    */

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
		// We are infact going to the next level...
		currWorld->IncrementLevel(this->gameModel);
		// Place the ball back on the paddle, and let the next level begin!
		this->gameModel->SetNextState(new LevelStartState(this->gameModel));
	}
}