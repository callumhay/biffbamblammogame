#ifndef __GAMEMODEL_H__
#define __GAMEMODEL_H__

#include "LevelPiece.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "PlayerPaddle.h"
#include "GameBall.h"
#include "GameState.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GameEventManager.h"

#include "../Utils/Vector.h"

#include <vector>
#include <algorithm>

class GameModel {

private:
	GameState* currState;		// The current game state

	// Player-controllable game assets
	PlayerPaddle playerPaddle;
	GameBall ball;

	// Current world and level information
	unsigned int currWorldNum;
	std::vector<GameWorld*> worlds;

	// Player score and life information
	int currPlayerScore;
	//int currScoreMultiplier;

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

	void BallCollisionOccurred(LevelPiece* p);

	// Increment the player's score in the game
	void IncrementScore(int amt) {
		bool wasGreaterThanZero = this->currPlayerScore > 0;

		this->currPlayerScore +=  amt;
		if (this->currPlayerScore < 0) {
				this->currPlayerScore = 0;
		}

		if (amt != 0 && wasGreaterThanZero){
			// EVENT: Score was changed
			GameEventManager::Instance()->ActionScoreChanged(amt);
		}
	}

public:	
	GameModel();
	~GameModel();

	// General public methods for the model ********************************
	void Tick(double seconds);

	// Level/World related queries *****************************************
	void BeginOrRestartGame();

	std::vector<std::vector<LevelPiece*>>& GetCurrentLevelPieces() {
		return this->GetCurrentWorld()->GetCurrentLevel()->GetCurrentLevelLayout();
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

	// Queries as to whether the current level is the last level of the game
	// Return: true if the model is on the last level of the game, false otherwise.
	bool IsOnLastLevelOfGame() const {
		return this->IsLastWorld() && this->GetCurrentWorld()->IsLastLevel();
	}

	// Paddle and ball related queries **************************************
	PlayerPaddle GetPlayerPaddle() const {
		return this->playerPaddle;
	}
	GameBall GetGameBall() const {
		return this->ball;
	}

	// Paddle and ball related manipulators *********************************

	// Move the paddle a distance in either positive or negative X direction.
	void MovePaddle(float dist) {
		this->currState->MovePaddleKeyPressed(dist);
	}
	// Release the ball from the paddle
	void ReleaseBall() {
		this->currState->BallReleaseKeyPressed();
	}

	// Befriend all state machine classes... 
	// I know it's ugly but it's how the stupid pattern works
	friend class GameState;
	friend class BallOnPaddleState;
	friend class BallInPlayState;
};

#endif