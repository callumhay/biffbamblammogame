#ifndef __GAMEMODEL_H__
#define __GAMEMODEL_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "GameModelConstants.h"
#include "LevelPiece.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "PlayerPaddle.h"
#include "GameBall.h"
#include "GameState.h"
#include "BallInPlayState.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GameEventManager.h"
#include "GameItem.h"
#include "GameItemTimer.h"
#include "GameItemFactory.h"
#include "Projectile.h"
#include "GameTransformMgr.h"

class GameModel {

private:
	GameState* currState;		// The current game state

	// Player-controllable game assets
	PlayerPaddle* playerPaddle;
	std::list<GameBall*> balls;

	// Projectiles spawned via various means as the game is played
	std::list<Projectile*> projectiles;

	// Current world and level information
	unsigned int currWorldNum;
	std::vector<GameWorld*> worlds;

	// Items that are currently available to be picked up in-game
	std::list<GameItem*> currLiveItems;
	// Timers that are currently active
	std::list<GameItemTimer*> activeTimers;

	// Player score and life information
	int currPlayerScore;
	int numConsecutiveBlocksHit;
	int currLivesLeft;

	// Whether or not the game is currently paused
	bool gameIsPaused;

	// Item related tracking variables *******************
	bool areControlsFlipped;
	bool isBlackoutActive;										// Is the game currently blacked out?
	GameTransformMgr gameTransformInfo;		// The current transform information of the game
	
	// ***************************************************

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

	void CollisionOccurred(Projectile* projectile, LevelPiece* p, bool& stateChanged);
	void CollisionOccurred(const GameBall& ball, LevelPiece* p, bool& stateChanged);
	void BallPaddleCollisionOccurred(GameBall& ball);
	void BallDied(GameBall* deadBall, bool& stateChanged);
	
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
		if (value < GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT) {
			value = GameModelConstants::GetInstance()->DEFAULT_BLOCKS_HIT;
		}

		if (value != this->numConsecutiveBlocksHit) {
			int oldMultiplier = this->numConsecutiveBlocksHit;
			this->numConsecutiveBlocksHit = value;
			
			// EVENT: The score multiplier has changed
			GameEventManager::Instance()->ActionScoreMultiplierChanged(oldMultiplier, this->numConsecutiveBlocksHit);
		}
	}

	void ClearProjectiles();
	void ClearLiveItems();
	void ClearActiveTimers();

public:	
	GameModel();
	~GameModel();

	// General public methods for the model ********************************
	void Tick(double seconds);

	// Level/World related queries *****************************************
	void BeginOrRestartGame();
	
	GameLevel* GetCurrentLevel() const {
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
	void SetLivesLeft(int lives) {
		assert(lives >= 0);

		int livesLeftBefore = this->currLivesLeft;
		this->currLivesLeft = lives;

		if (this->currLivesLeft > GameModelConstants::GetInstance()->MAX_LIVES_LEFT) {
			this->currLivesLeft = GameModelConstants::GetInstance()->MAX_LIVES_LEFT;
		}
		else {
			GameEventManager::Instance()->ActionLivesChanged(livesLeftBefore, this->currLivesLeft);
		}
	}

	std::list<GameItem*>& GetLiveItems() {
		return this->currLiveItems;
	}

	std::list<GameItemTimer*>& GetActiveTimers() {
		return this->activeTimers;
	}

	std::list<Projectile*>& GetActiveProjectiles() {
		return this->projectiles;
	}

	bool IsGameOver() const {
		return this->currLivesLeft == 0;
	}

	GameTransformMgr& GetTransformInfo() {
		return this->gameTransformInfo;
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
	PlayerPaddle* GetPlayerPaddle() const {
		return this->playerPaddle;
	}
	std::list<GameBall*>& GetGameBalls() {
		return this->balls;
	}

	// Paddle and ball related manipulators *********************************

	// Move the paddle a distance in either positive or negative X direction.
	void MovePaddle(float dist) {
		if (this->currState != NULL) {
			this->currState->MovePaddleKeyPressed(dist);
		}
	}
	// Release the ball from the paddle
	void ReleaseBall() {
		if (this->currState != NULL) {
			this->currState->BallReleaseKeyPressed();
		}
	}

	// Pauses the game
	void TogglePauseGame() {
		this->gameIsPaused = !this->gameIsPaused;
	}

	// Item effect related getters and setters ***************************

	// Sets whether the game is currently blacked out or not
	void SetBlackoutEffect(bool isActive) {
		this->isBlackoutActive = isActive;
	}
	// Gets whether the game is currently blacked out or not
	bool IsBlackoutEffectActive() const {
		return this->isBlackoutActive;
	}

	// Sets whether the controls for the paddle are flipped or not
	void SetFlippedControls(bool flippedControlsOn) {
		this->areControlsFlipped = flippedControlsOn;
	}
	// Gets whether the controls for the paddle are flipped
	bool AreControlsFlipped() const {
		return this->areControlsFlipped;
	}

	// *******************************************************************

	void AddPossibleItemDrop(LevelPiece* p);
	void AddProjectile(Projectile::ProjectileType type, const Point2D& spawnLoc);

	// Debug functions
#ifdef _DEBUG
	void DropItem(const std::string& itemName) {
		BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
		if (state != NULL) {
			Vector2D levelDim = this->GetLevelUnitDimensions();
			state->DebugDropItem(GameItemFactory::CreateItem(itemName, Point2D(0,0) + 0.5f*levelDim, this));
		}
	}
#endif

	// Befriend all state machine classes... 
	// I know it's ugly but it's how the stupid pattern works
	friend class GameState;
	friend class BallOnPaddleState;
	friend class BallInPlayState;
};

#endif