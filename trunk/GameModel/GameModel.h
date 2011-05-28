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
#include "PointAward.h"
#include "BallBoostModel.h"

class CollateralBlock;

class GameModel {

private:
	GameState* currState;		// The current game state
	GameState* nextState;

	// Player-controllable game assets
	PlayerPaddle* playerPaddle;                         // The one and only player paddle
	std::list<GameBall*> balls;                         // Current set of balls active in the game
	std::list<Projectile*> projectiles;                 // Projectiles spawned as the game is played
	std::list<Beam*> beams;                             // Beams spawned as the game is played
	std::map<LevelPiece*, int32_t> statusUpdatePieces;  // Pieces that require updating every frame due to status effects
    BallBoostModel* boostModel;                         // This is only not NULL when the ball is in play - it contains the model/state for ball boosting

	// Current world and level information
	unsigned int currWorldNum;
	std::vector<GameWorld*> worlds;

	// Items that are currently available to be picked up in-game
	std::list<GameItem*> currLiveItems;
	// Timers that are currently active
	std::list<GameItemTimer*> activeTimers;

	// Player score and life information
	long currPlayerScore;
	int numInterimBlocksDestroyed;
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

	bool doingPieceStatusListIteration;

	// Private getters and setters ****************************************
	void SetCurrentWorldAndLevel(int worldNum, int levelNum);

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
	void DoPieceStatusUpdates(double dT);
	void DoProjectileCollisions();
	
	bool IsOutOfGameBounds(const Point2D& pos);

	void UpdateActiveTimers(double seconds);
	void UpdateActiveItemDrops(double seconds);
	void UpdateActiveProjectiles(double seconds);
	void UpdateActiveBeams(double seconds);

    void ResetScore();
	void SetNumInterimBlocksDestroyed(int value, const Point2D& pos = Point2D());
    int GetNumInterimBlocksDestroyed() const { return this->numInterimBlocksDestroyed; }
    int GetCurrentMultiplier() const;

	void ClearProjectiles();
	void ClearBeams();
	void ClearLiveItems();
	void ClearActiveTimers();
	void ClearStatusUpdatePieces();
    void ClearBallsToOne();

	bool RemoveActiveGameItemsOfGivenType(const GameItem::ItemType& type);
    void RemoveActiveGameItemsForThisBallOnly(const GameBall* ball);
    
public:
	// Pause functionality for the game:
	// NoPause: no pauses at all in the game model execution.
	// PauseState: Only the current state is paused - thus if the game is in play no movement of paddle, ball, items, etc. will occur.
	// PausePaddle: Only pauses the paddle movement, animations, etc.
	// PauseGame: Pauses the entire game - should be used when the user pauses the game.
	// AllPause: All possible pauses are active
	enum PauseType { NoPause = 0x00000000, PauseState = 0x00000001, PausePaddle = 0x00000002, 
                     PauseGame = 0x80000000, PauseBall = 0x00000004, AllPause = 0xFFFFFFFF };

	GameModel();
	~GameModel();

    void IncrementScore(PointAward& pointAward);
    void IncrementScore(std::list<PointAward>& pointAwardsList);
    void IncrementNumInterimBlocksDestroyed(const Point2D& pos) { this->SetNumInterimBlocksDestroyed(this->GetNumInterimBlocksDestroyed()+1, pos); }

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
    void StartGameAtWorldAndLevel(int worldNum, int levelNum);
	void BeginOrRestartGame();
	void ClearGameState();
	
    const std::vector<GameWorld*>& GetGameWorlds() const {
        return this->worlds;
    }

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

	long GetScore() const {
		return this->currPlayerScore;
	}

	int GetNumConsecutiveBlocksHit() const {
		return this->numInterimBlocksDestroyed;
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

	void WipePieceFromAuxLists(LevelPiece* piece);

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
			if ((currBall->GetBallType() & effectType) == static_cast<int32_t>(effectType)) {
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
    const std::list<GameBall*>& GetGameBalls() const {
        return this->balls;
    }

	// Paddle and ball related manipulators *********************************

	// Move the paddle a distance in either positive or negative X direction.
	void MovePaddle(size_t frameID, const PlayerPaddle::PaddleMovement& paddleMovement) {
		static PlayerPaddle::PaddleMovement lastPaddleMoveThisFrame = PlayerPaddle::NoPaddleMovement;
		static size_t lastFrameID = 0;
		if (frameID == lastFrameID) {
			// We ignore 'no movement' in cases where the paddle has been told to move this frame
			if (lastPaddleMoveThisFrame != PlayerPaddle::NoPaddleMovement && paddleMovement == PlayerPaddle::NoPaddleMovement) {
				return;
			}
		}
		else {
			lastFrameID = frameID;
		}
		lastPaddleMoveThisFrame = paddleMovement;

		// Can only move the paddle if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
				 this->currState->MovePaddleKeyPressed(paddleMovement);
		}
	}

	// Release the ball from the paddle, shoot lasers and activate other power ups
	void ShootActionReleaseUse() {
		// Can only release the ball if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
			this->currState->BallReleaseKeyPressed();
		}
	}

    void BallBoostDirectionPressed(int x, int y) {
		// Can only do this if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
			this->currState->BallBoostDirectionPressed(x, y);
		}
    }
    void BallBoostDirectionReleased() {
		// Can only do this if the state exists and is not paused
		if (this->currState != NULL && (this->pauseBitField & GameModel::PausePaddle) == 0x0 &&
			 (this->pauseBitField & GameModel::PauseState) == 0x0) {
			this->currState->BallBoostDirectionReleased();
		}
    }
    float GetTimeDialationFactor() const {
        if (this->boostModel == NULL) { return 1.0f; }
        return this->boostModel->GetTimeDialationFactor();
    }
    const BallBoostModel* GetBallBoostModel() const {
        return this->boostModel;
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

	bool AddStatusUpdateLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status);
	bool RemoveStatusForLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status);

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

inline void GameModel::ResetScore() {
    this->currPlayerScore = GameModelConstants::GetInstance()->INIT_SCORE;
    // EVENT: Score was changed
    GameEventManager::Instance()->ActionScoreChanged(this->currPlayerScore);
}

inline void GameModel::ClearStatusUpdatePieces() {
	assert(!this->doingPieceStatusListIteration);
	// When we clear all the update pieces we also clear all the relevant status effects for those pieces...
	this->doingPieceStatusListIteration = true;
	for (std::map<LevelPiece*, int32_t>::iterator iter = this->statusUpdatePieces.begin(); iter != this->statusUpdatePieces.end(); ++iter) {
		LevelPiece* currLevelPiece = iter->first;
		currLevelPiece->RemoveStatuses(iter->second);
	}
	this->statusUpdatePieces.clear();
	this->doingPieceStatusListIteration = false;
}

#endif