/**
 * GameModel.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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


class CollateralBlock;

class GameModel {

private:
	GameState* currState;		// The current game state
	GameState* nextState;

	// Player-controllable game assets
	PlayerPaddle* playerPaddle;						// The one and only player paddle
	std::list<GameBall*> balls;						// Current set of balls active in the game
	std::list<Projectile*> projectiles;		// Projectiles spawned as the game is played
	std::list<Beam*> beams;								// Beams spawned as the game is played

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

	GameWorld* GetCurrentWorld() const {
		return this->worlds[this->currWorldNum];
	}

	void SetCurrentStateImmediately(GameState* newState) {
		assert(newState != NULL);
		if (this->currState != NULL) {
			delete this->currState;
		}
		this->currState = newState;
	}

	void CollisionOccurred(Projectile* projectile, LevelPiece* p);
	void CollisionOccurred(GameBall& ball, LevelPiece* p);
	void CollisionOccurred(Projectile* projectile, PlayerPaddle* paddle);
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
	void ClearBeams();
	void ClearLiveItems();
	void ClearActiveTimers();

	bool RemoveActiveGameItemsOfGivenType(const GameItem::ItemType& type);

public:
	// Pause functionality for the game:
	// NoPause: no pauses at all in the game model execution.
	// PauseState: Only the current state is paused - thus if the game is in play no movement of paddle, ball, items, etc. will occur.
	// PausePaddle: Only pauses the paddle movement, animations, etc.
	// PauseGame: Pauses the entire game - should be used when the user pauses the game.
	// AllPause: All possible pauses are active
	enum PauseType { NoPause = 0x00000000, PauseState = 0x00000001, PausePaddle = 0x00000002, PauseGame = 0x80000000, PauseBall = 0x00000004, AllPause = 0xFFFFFFFF };

	GameModel();
	~GameModel();

	GameState::GameStateType GetCurrentStateType() const {
		if (this->currState != NULL) {
			return this->currState->GetType();
		}
		return GameState::NULLStateType;
	}
	
	// The given next state will become the next state when update state is called
	void SetNextState(GameState* nextState) {
		if (this->nextState != NULL) {
			delete this->nextState;
		}
		this->nextState = nextState;
	}
	// Update the current state to the next state if there is a next state set
	void UpdateState() {
		if (this->nextState != NULL) {
			this->SetCurrentStateImmediately(this->nextState);
			this->nextState = NULL;
		}
	}

	// General public methods for the model ********************************
	void Tick(double seconds);

	// Level/World related queries *****************************************
	void BeginOrRestartGame();
	void ClearGameState();
	
	GameLevel* GetCurrentLevel() const {
		return this->worlds[this->currWorldNum]->GetCurrentLevel();
	}

	GameWorld::WorldStyle GetCurrentWorldStyle() const {
		return this->GetCurrentWorld()->GetStyle();
	}
	const std::string& GetCurrentWorldName() const {
		return this->GetCurrentWorld()->GetName();
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
		
		if (livesLeftBefore != this->currLivesLeft) {
			// EVENT: Number of lives just changed
			GameEventManager::Instance()->ActionLivesChanged(livesLeftBefore, this->currLivesLeft);
		}
	}
	void SetInitialNumberOfLives(int lives) {
		assert(lives <= GameModelConstants::GetInstance()->MAX_LIVES_LEFT);
		this->currLivesLeft = lives;
		// EVENT: Number of lives just changed
		GameEventManager::Instance()->ActionLivesChanged(0, this->currLivesLeft);
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

	std::list<Beam*>& GetActiveBeams() {
		return this->beams;
	}
	const std::list<Beam*>& GetActiveBeams() const {
		return this->beams;
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
	void MovePaddle(const PlayerPaddle::PaddleMovement& paddleMovement) {
		// Can only move the paddle if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
				 this->currState->MovePaddleKeyPressed(paddleMovement);
		}
	}
	// Release the ball from the paddle, shoot lasers and activate other power ups
	void ReleaseBall() {
		// Can only release the ball if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
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
	void SetPauseState(int pauseState) {
		this->pauseBitField = pauseState;
	}
	int GetPauseState() const {
		return this->pauseBitField;
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

	void AddPossibleItemDrop(const LevelPiece& p);
	void AddItemDrop(const LevelPiece& p, const GameItem::ItemType& itemType);
	void AddProjectile(Projectile* projectile);
	void AddBeam(int beamType);

	// Debug functions
#ifdef _DEBUG
	void DropItem(GameItem::ItemType itemType) {
		BallInPlayState* state = dynamic_cast<BallInPlayState*>(this->currState);
		if (state != NULL) {
			Vector2D levelDim = this->GetLevelUnitDimensions();
			state->DebugDropItem(GameItemFactory::GetInstance()->CreateItem(itemType, Point2D(0,0) + 0.5f*levelDim, this));
		}
	}
#endif

	// Befriend all state machine classes... 
	// I know it's ugly but it's how the stupid pattern works
	friend class GameState;
	friend class BallOnPaddleState;
	friend class BallInPlayState;
	friend class LevelCompleteState;
	friend class WorldCompleteState;

	friend class BallDeathState;
	friend class GameOverState;
};

#endif