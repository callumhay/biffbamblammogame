#include "LevelCompleteState.h"
#include "WorldCompleteState.h"
#include "BallOnPaddleState.h"
#include "GameModel.h"

/**
 * When a level completes the game model needs to be set to the appropriate
 * set of variable states so it can continue on to the next level and clear
 * all the craziness of the previous level's state...
 */
LevelCompleteState::LevelCompleteState(GameModel* gm) : GameState(gm) {
	GameWorld* currWorld = this->gameModel->GetCurrentWorld();
	GameLevel* currLevel = currWorld->GetCurrentLevel();
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

	// Clean up all the level-related state stuff in preparation for the next level/world/?
	
	// Reset the multiplier
	this->gameModel->SetNumInterimBlocksDestroyed(0);
	// Reset the game transform manager
	this->gameModel->GetTransformInfo()->Reset();

	// Set the number of balls that exist to just 1
	std::list<GameBall*>& gameBalls = this->gameModel->GetGameBalls();
	if (gameBalls.size() > 1) {
		std::list<GameBall*>::iterator ballIter = gameBalls.begin();
		++ballIter;
		for (; ballIter != gameBalls.end(); ++ballIter) {
			GameBall* ballToDestroy = *ballIter;
			delete ballToDestroy;
			ballToDestroy = NULL;
		}

		// Grab the last ball left and make it the only element in the entire game balls array
		GameBall* onlyBallLeft = (*gameBalls.begin());
		assert(onlyBallLeft != NULL);
		gameBalls.clear();
		gameBalls.push_back(onlyBallLeft);
	}

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
		this->gameModel->SetNextState(new BallOnPaddleState(this->gameModel));
	}
}