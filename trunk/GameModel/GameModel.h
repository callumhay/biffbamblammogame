#ifndef __GAMEMODEL_H__
#define __GAMEMODEL_H__

#include "../BlammoEngine/BasicIncludes.h"

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

	// Holds the type of pausing that is currently being used on the game model (see PauseType enum)
	int pauseBitField;

	bool gameIsEntirelyPaused;
	bool gameStateIsPaused;

	// Item related tracking variables *******************
	bool areControlsFlipped;
	bool isBlackoutActive;										// Is the game currently blacked out?
	GameTransformMgr* gameTransformInfo;		// The current transform information of the game
	
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
	// Pause functionality for the game:
	// NoPause: no pauses at all in the game model execution.
	// PauseState: Only the current state is paused - thus if the game is in play no movement of paddle, ball, items, etc. will occur.
	// PausePaddle: Only pauses the paddle movement, animations, etc.
	// PauseGame: Pauses the entire game - should be used when the user pauses the game.
	// AllPause: All possible pauses are active
	enum PauseType { NoPause = 0x00000000, PauseState = 0x00000001, PausePaddle = 0x00000002, PauseGame = 0x80000000, AllPause = 0xFFFFFFFF };

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

	/**
	 * Determines whether the given ball effect is active in the current state of the game or not.
	 * Returns: true if the ball effect is active on any one of the balls in play, false otherwise.
	 */
	bool IsBallEffectActive(GameBall::BallType effectType) const {
		for (std::list<GameBall*>::const_iterator ballIter = this->balls.begin(); ballIter != this->balls.end(); ++ballIter) {
			const GameBall* currBall = *ballIter;
			if ((currBall->GetBallType() & effectType) == effectType) {
				return true;
			}
		}
		return false;
	}

	bool IsGameOver() const {
		return this->currLivesLeft == 0;
	}

	GameTransformMgr* GetTransformInfo() const {
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
		// Can only move the paddle if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::AllPause) == NULL) {
			this->currState->MovePaddleKeyPressed(dist);
		}
	}
	// Release the ball from the paddle
	void ReleaseBall() {
		// Can only release the ball if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::AllPause) == NULL) {
			this->currState->BallReleaseKeyPressed();
		}
	}

	// Pauses the game
	void SetPause(PauseType pause) {
		this->pauseBitField |= pause;
	}
	void UnsetPause(PauseType pause) {
		this->pauseBitField &= ~pause;
	}
	void TogglePause(PauseType pause) {
		if ((this->pauseBitField & pause) == pause) {
			this->UnsetPause(pause);
		}
		else {
			this->SetPause(pause);
		}
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
	void DropItem(GameItem::ItemType itemType) {
		BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
		if (state != NULL) {
			Vector2D levelDim = this->GetLevelUnitDimensions();
			state->DebugDropItem(GameItemFactory::CreateItem(itemType, Point2D(0,0) + 0.5f*levelDim, this));
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