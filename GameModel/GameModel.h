#ifndef __GAMEMODEL_H__
#define __GAMEMODEL_H__

#include "GameConstants.h"
#include "LevelPiece.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "PlayerPaddle.h"
#include "GameBall.h"
#include "GameState.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GameEventManager.h"
#include "GameItem.h"

#include "../Utils/Vector.h"

#include <vector>
#include <algorithm>

class GameModel {

private:
	GameState* currState;		// The current game state

	// Player-controllable game assets
	PlayerPaddle *playerPaddle;
	GameBall *ball;

	// Current world and level information
	unsigned int currWorldNum;
	std::vector<GameWorld*> worlds;

	// Items that are currently available to be picked up in-game
	std::vector<GameItem*> currLiveItems;

	// Player score and life information
	int currPlayerScore;
	int numConsecutiveBlocksHit;
	int currLivesLeft;

	// Private getters and setters ****************************************
	void SetCurrentWorld(unsigned int worldNum);
	void IncrementLevel();

	GameWorld* GetCurrentWorld() const {
		return this->worlds[this->currWorldNum];
	}

	void SetCurrentState(GameState* nextState) {
		assert(nextState != NULL);
		if (this->currState != NULL) {
			delete this->currState;
		}
		this->currState = nextState;
	}

	void BallPieceCollisionOccurred(LevelPiece* p);
	void BallPaddleCollisionOccurred();
	void PlayerDied();

	// Increment the player's score in the game
	void IncrementScore(int amt) {
		bool wasGreaterThanZero = this->currPlayerScore > 0;
		int amtChanged = amt;
			
		this->currPlayerScore +=  amtChanged;
		if (this->currPlayerScore < 0) {
				this->currPlayerScore = 0;
		}

		if (amt != 0 && wasGreaterThanZero){
			// EVENT: Score was changed
			GameEventManager::Instance()->ActionScoreChanged(amtChanged);
		}
	}

	// Set the number of consecutive blocks hit by the ball in the interrum between
	// when it leaves and returns to the player paddle
	void SetNumConsecutiveBlocksHit(int value) {
		if (value < GameConstants::DEFAULT_BLOCKS_HIT) {
			value = GameConstants::DEFAULT_BLOCKS_HIT;
		}

		if (value != this->numConsecutiveBlocksHit) {
			int oldMultiplier = this->numConsecutiveBlocksHit;
			this->numConsecutiveBlocksHit = value;
			
			// EVENT: The score multiplier has changed
			GameEventManager::Instance()->ActionScoreMultiplierChanged(oldMultiplier, this->numConsecutiveBlocksHit);
		}
	}

	void ClearLiveItems();

public:	
	GameModel();
	~GameModel();

	// General public methods for the model ********************************
	void Tick(double seconds);

	// Level/World related queries *****************************************
	void BeginOrRestartGame();
	
	const GameLevel* GetCurrentLevel() const {
		return this->worlds[this->currWorldNum]->GetCurrentLevel();
	}

	GameWorld::WorldStyle GetCurrentWorldStyle() const {
		return this->GetCurrentWorld()->GetStyle();
	}
	
	Vector2D GetLevelUnitDimensions() const {
		GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
		assert(currLevel != NULL);

		return Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
	}

	bool IsLastWorld() const {
		return this->currWorldNum == this->worlds.size()-1;
	}

	int GetScore() const {
		return this->currPlayerScore;
	}

	int GetNumConsecutiveBlocksHit() const {
		return this->numConsecutiveBlocksHit;
	}

	int GetLivesLeft() const {
		return this->currLivesLeft;
	}

	std::vector<GameItem*>& GetLiveItems() {
		return this->currLiveItems;
	}

	bool IsGameOver() const {
		return this->currLivesLeft < 0;
	}

	// Queries as to whether the current level is the last level of the game
	// Return: true if the model is on the last level of the game, false otherwise.
	bool IsOnLastLevelOfGame() const {
		return this->IsLastWorld() && this->GetCurrentWorld()->IsLastLevel();
	}

	// Paddle and ball related queries **************************************
	PlayerPaddle* GetPlayerPaddle() {
		return this->playerPaddle;
	}
	GameBall* GetGameBall() {
		return this->ball;
	}

	// Paddle and ball related manipulators *********************************

	// Move the paddle a distance in either positive or negative X direction.
	void MovePaddle(float dist) {
		if (currState != NULL) {
			this->currState->MovePaddleKeyPressed(dist);
		}
	}
	// Release the ball from the paddle
	void ReleaseBall() {
		if (currState != NULL) {
			this->currState->BallReleaseKeyPressed();
		}
	}

	// Befriend all state machine classes... 
	// I know it's ugly but it's how the stupid pattern works
	friend class GameState;
	friend class BallOnPaddleState;
	friend class BallInPlayState;
};

#endif