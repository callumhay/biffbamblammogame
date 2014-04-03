/**
 * GameModel.h
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
#include "GameLevel.h"
#include "GameWorld.h"
#include "GameEventManager.h"
#include "GameItem.h"
#include "GameItemTimer.h"
#include "GameItemFactory.h"
#include "Projectile.h"
#include "BallBoostModel.h"
#include "Beam.h"

class GameSound;
class BallInPlayState;
class CollateralBlock;
class LevelStartState;
class PointAward;
class SafetyNet;
class PaddleRemoteControlRocketProjectile;
class GameTransformMgr;

class GameModel {

    // Befriend all state machine classes... I know it's ugly but it's how the pattern works
    friend class GameState;
    friend class BallOnPaddleState;
    friend class BallInPlayState;
    friend class LevelStartState;
    friend class LevelCompleteState;
    friend class WorldCompleteState;
    friend class BallDeathState;
    friend class GameOverState;

public:
    typedef std::list<Projectile*> ProjectileList;
    typedef ProjectileList::const_iterator ProjectileListConstIter;
    typedef ProjectileList::iterator ProjectileListIter;
    typedef std::map<Projectile::ProjectileType, ProjectileList> ProjectileMap;
    typedef ProjectileMap::const_iterator ProjectileMapConstIter;
    typedef ProjectileMap::iterator ProjectileMapIter;

	// Pause functionality for the game:
	// NoPause: no pauses at all in the game model execution.
	// PauseState: Only the current state is paused - thus if the game is in play no movement of paddle, ball, items, etc. will occur.
	// PausePaddle: Only pauses the paddle movement, animations, etc.
	// PauseGame: Pauses the entire game - should be used when the user pauses the game.
	// AllPause: All possible pauses are active
	enum PauseType { NoPause = 0x00000000, PauseState = 0x00000001, PausePaddle = 0x00000002, 
                     PauseGame = 0x80000000, PauseBall = 0x00000004, PauseAI = 0x00000008, 
                     PausePaddleControls = 0x00000010, PauseTimers = 0x00000020, AllPause = 0xFFFFFFFF };

    // Difficulty of the game:
    // EasyDifficulty: The game is significantly easier to play - ball moves slower, more boost time, no
    // paddle timer when releasing the ball
    // MediumDifficulty: The game is typical difficulty, ball moves normal speed, typical boost time,
    // there is a paddle timer when releasing the ball
    // HardDifficulty: The game is quite difficult, ball moves a bit faster than normal, boost time is quite short,
    // there is a paddle timer when releasing the ball
    enum Difficulty { EasyDifficulty = 0, MediumDifficulty = 1, HardDifficulty = 2};
    
    GameModel(GameSound* sound, const GameModel::Difficulty& initDifficulty, bool ballBoostIsInverted,
        const BallBoostModel::BallBoostMode& ballBoostMode);
	~GameModel();

    bool IsOutOfGameBoundsForBall(const Point2D& pos) const;
    bool IsOutOfGameBoundsForItem(const Point2D& pos) const;
    bool IsOutOfGameBoundsForProjectile(const Point2D& pos) const;
    bool IsOutOfPaddedLevelBounds(const Point2D& pos, float padding) const;
    bool IsOutOfPaddedLevelBounds(const Point2D& pos, float xPadding, float yPadding) const;
    bool IsOutOfPaddedLevelBounds(const Point2D& pos, float minXPadding, float minYPadding, float maxXPadding, float maxYPadding) const;
    
    Collision::AABB2D GenerateLevelProjectileBoundaries() const;

    void GetFurthestProgressWorldAndLevel(int& worldIdx, int& levelIdx) const;
    void OverrideGetFurthestProgress(int& worldIdx, int& levelIdx);
    
    static bool IsTutorialLevel(int worldIdx, int levelIdx) {
        return (worldIdx == 0 && levelIdx == 0);
    }
    bool IsCurrentLevelTheTutorialLevel() const {
        const GameWorld* world = this->GetCurrentWorld();
        const GameLevel* level = this->GetCurrentLevel();
        // Zeroth world, zeroth level == tutorial level
        return GameModel::IsTutorialLevel(world->GetWorldIndex(), level->GetLevelIndex());
    }
    bool IsCurrentLevelABossLevel() const {
        const GameLevel* level = this->GetCurrentLevel();
        return level->GetHasBoss(); 
    }

    void IncrementScore(PointAward& pointAward);
    //void IncrementScore(std::list<PointAward>& pointAwardsList);
    void IncrementNumInterimBlocksDestroyed(const Point2D& pos) { this->SetNumInterimBlocksDestroyed(this->GetNumInterimBlocksDestroyed()+1, pos); }

    GameWorld* GetWorldByName(const std::string& name) const;

    GameWorld* GetWorldByIndex(int index) const {
        assert(index >= 0 && index < static_cast<int>(this->worlds.size()));
        return this->worlds[index];
    }

	GameWorld* GetCurrentWorld() const {
		return this->worlds[this->currWorldNum];
	}

    GameSound* GetSound() const {
        return this->sound;
    }

	GameState::GameStateType GetCurrentStateType() const {
		if (this->currState != NULL) {
			return this->currState->GetType();
		}
		return GameState::NULLStateType;
	}

    void SetDifficulty(const GameModel::Difficulty& difficulty);
    const GameModel::Difficulty& GetDifficulty() const { return this->difficulty; }

	// The given next state will become the next state when update state is called
    void SetNextState(const GameState::GameStateType& nextStateType) {
		if (this->nextState != NULL) {
            // Make sure we aren't setting the same state twice
            if (nextStateType == this->nextState->GetType()) {
                return;
            }

			delete this->nextState;
		}
        this->nextState = GameState::Build(nextStateType, this);
	}
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

    bool IsBottomSafetyNetActive() const { return this->bottomSafetyNet != NULL; }
    bool IsTopSafetyNetActive() const { return this->topSafetyNet != NULL; }
    bool IsBottomSafetyNet(const SafetyNet* safetyNet) { return safetyNet != NULL && this->bottomSafetyNet == safetyNet; }

    bool ActivateSafetyNet();
    void DestroySafetyNet(SafetyNet* netToDestroy);
    void DestroyBottomSafetyNet();
    void DestroyTopSafetyNet();

    void CleanUpAfterLevelEnd();

	// General public methods for the model ********************************
	void Tick(double seconds);

	// Level/World related queries *****************************************
    void StartGameAtWorldAndLevel(int worldIdx, int levelIdx);
    void ResetCurrentLevel();
	void ClearGameState();
    void ClearAllGameProgress();
	
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
    int GetLastWorldIndex() const {
        return static_cast<int>(this->worlds.size()) - 1;
    }
	
    void IncrementCurrentWorldNum() {
        assert(!this->IsLastWorld());
        this->currWorldNum++; 
    }

    int GetTotalStarsCollectedInGame() const;

	Vector2D GetLevelUnitDimensions() const {
		GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
		assert(currLevel != NULL);

		return Vector2D(currLevel->GetLevelUnitWidth(), currLevel->GetLevelUnitHeight());
	}
    Vector3D GetCurrentLevelTranslation() const {
        GameLevel* currLevel = this->GetCurrentWorld()->GetCurrentLevel();
        assert(currLevel != NULL);
        return currLevel->GetTranslationToMiddle();
    }

	bool IsLastWorld() const {
		return this->currWorldNum == this->worlds.size()-1;
	}

	long GetScore() const {
		return this->currPlayerScore;
	}

    int GetNumStarsAwarded() const {
        return this->numStarsAwarded;
    }

	int GetNumConsecutiveBlocksHit() const {
		return this->numInterimBlocksDestroyed;
	}
    int GetMaxConsecutiveBlocksDestroyed() const { return this->maxInterimBlocksDestroyed; }
    int GetNumLivesLostInCurrentLevel() const { return this->numLivesLostInLevel; }
    void GetNumItemsAcquired(int& numGoodItems, int& numNeutralItems, int& numBadItems) {
        numGoodItems = this->numGoodItemsAcquired;
        numNeutralItems = this->numNeutralItemsAcquired;
        numBadItems = this->numBadItemsAcquired;
    }

    double GetLevelTimeInSeconds() const { return this->totalLevelTimeInSeconds; }

	int GetLivesLeft() const {
		return this->currLivesLeft;
	}
    int GetLivesAtStartOfLevel() const {
        return this->livesAtStartOfLevel;
    }
	void SetLivesLeft(int lives, bool triggerEvent = true) {
		assert(lives >= 0);

		int livesLeftBefore = this->currLivesLeft;
		this->currLivesLeft = lives;

		if (this->currLivesLeft > this->maxNumLivesAllowed) {
			this->currLivesLeft = this->maxNumLivesAllowed;
		}
		
		if (livesLeftBefore != this->currLivesLeft && triggerEvent) {
			// EVENT: Number of lives just changed
			GameEventManager::Instance()->ActionLivesChanged(livesLeftBefore, this->currLivesLeft);
		}
	}
	void SetInitialNumberOfLives(int lives) {
        assert(lives <= GameModelConstants::GetInstance()->MAXIMUM_POSSIBLE_LIVES);
		
        this->currLivesLeft = lives;
        this->maxNumLivesAllowed = lives;

		// EVENT: Number of lives just changed
		GameEventManager::Instance()->ActionLivesChanged(0, this->currLivesLeft);
	}

	void WipePieceFromAuxLists(LevelPiece* piece);

    void ClearSpecificBeams(const Beam::BeamType& beamType);

	std::list<GameItem*>& GetLiveItems() {
		return this->currLiveItems;
	}

	std::list<GameItemTimer*>& GetActiveTimers() {
		return this->activeTimers;
	}
    bool IsTimerTypeActive(const GameItem::ItemType& type) const;

    GameModel::ProjectileMap& GetActiveProjectiles() {
		return this->projectiles;
	}
    const GameModel::ProjectileList* GetActiveProjectilesWithType(Projectile::ProjectileType type) const {
        GameModel::ProjectileMapConstIter findIter = this->projectiles.find(type);
        if (findIter == this->projectiles.end()) { return NULL; }
        return &findIter->second;
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

    GameBall* GetBallChosenForBallCamera() const {
        return this->GetGameBalls().front();
    }

	bool IsGameOver() const {
		return this->currLivesLeft == 0;
	}

	GameTransformMgr* GetTransformInfo() const {
		return this->gameTransformInfo;
	}

    bool GetIsUnusualCameraModeActive() const;

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
    Point2D GetAvgBallLoc() const;

    Vector3D GetGravityDir() const;
    Vector3D GetGravityRightDir() const;

    float GetGhostPaddleDistance() const;

	// Paddle and ball related manipulators *********************************

	// Move the paddle or some other interactive element that is active in the game...
	void MovePaddle(size_t frameID, int dir, float magnitudePercent = 1.0f) {
        assert(dir <= 1 && dir >= -1);
		
        // NOTE: The following code is used to 'clean-up' movements so that we don't over send
        // commands to the interactive elements of the game, instead we limit movement commands 
        // to once per simulated frame/tick of the game.
        static int lastMoveThisFrame = dir;
		static size_t lastFrameID = 0;
		if (frameID == lastFrameID) {
			// We ignore 'no movement' in cases where a movement has already been sent this frame
            if (lastMoveThisFrame != 0 && dir == 0) {
				return;
			}
		}
		else {
			lastFrameID = frameID;
		}
		lastMoveThisFrame = dir;

		// Can only move if the state exists and is not paused
		if (this->currState != NULL &&
		    (this->pauseBitField & GameModel::PauseState) == 0x0  &&
            (this->pauseBitField & GameModel::PauseGame) == 0x0) {

		    this->currState->MoveKeyPressedForPaddle(dir, magnitudePercent);
		}
	}
    void MoveOther(size_t frameID, int dir, float magnitudePercent = 1.0f) {
        assert(dir <= 1 && dir >= -1);

        // NOTE: The following code is used to 'clean-up' movements so that we don't over send
        // commands to the interactive elements of the game, instead we limit movement commands 
        // to once per simulated frame/tick of the game.
        static int lastMoveThisFrame = dir;
        static size_t lastFrameID = 0;
        if (frameID == lastFrameID) {
            // We ignore 'no movement' in cases where a movement has already been sent this frame
            if (lastMoveThisFrame != 0 && dir == 0) {
                return;
            }
        }
        else {
            lastFrameID = frameID;
        }
        lastMoveThisFrame = dir;

        // Can only move if the state exists and is not paused
        if (this->currState != NULL &&
            (this->pauseBitField & GameModel::PauseState) == 0x0  &&
            (this->pauseBitField & GameModel::PauseGame) == 0x0) {

            this->currState->MoveKeyPressedForOther(dir, magnitudePercent);
        }
    }

	// Release the ball from the paddle, shoot lasers and activate other power ups
	void ShootActionReleaseUse() {
		if (this->currState != NULL &&
			(this->pauseBitField & GameModel::PauseState) == 0x0 &&
            (this->pauseBitField & GameModel::PauseGame) == 0x0) {

		    this->currState->ShootActionReleaseUse();
		}
	}

    void ShootActionContinuousUse(double dT, float magnitudePercent = 1.0f) {
        if (this->currState != NULL &&
            (this->pauseBitField & GameModel::PauseState) == 0x0 &&
            (this->pauseBitField & GameModel::PauseGame) == 0x0) {

            this->currState->ShootActionContinuousUse(dT, magnitudePercent);
        }
    }

    void ToggleAllowPaddleBallLaunching(bool allow);

    const BallBoostModel::BallBoostMode& GetBallBoostMode() const { return this->ballBoostMode; }
    void SetBallBoostMode(const BallBoostModel::BallBoostMode& mode) { this->ballBoostMode = mode; }

    void BallBoostDirectionPressed(float x, float y);
    void BallBoostDirectionReleased();
    void BallBoostReleasedForBall(const GameBall& ball);
    void ReleaseBulletTime();

    float GetTimeDialationFactor() const;
    const BallBoostModel* GetBallBoostModel() const {
        return this->boostModel;
    }
    bool GetIsBallBoostInverted() const { return this->ballBoostIsInverted; }
    void SetInvertBallBoostDir(bool isInverted);
    float GetPercentBallReleaseTimerElapsed() const;

    void AddPercentageToBoostMeter(double percent);

	// Pauses the game
	void SetPause(PauseType pause) {
        int32_t prevPauseState = this->pauseBitField;
		this->pauseBitField |= pause;
        GameEventManager::Instance()->ActionGamePauseStateChanged(prevPauseState, this->pauseBitField);
	}
	void UnsetPause(PauseType pause) {
        int32_t prevPauseState = this->pauseBitField;
		this->pauseBitField &= ~pause;
        GameEventManager::Instance()->ActionGamePauseStateChanged(prevPauseState, this->pauseBitField);
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
        int32_t prevPauseState = this->pauseBitField;
		this->pauseBitField = pauseState;
        GameEventManager::Instance()->ActionGamePauseStateChanged(prevPauseState, this->pauseBitField);
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
	bool AreControlsFlippedForPaddle() const {
        return this->areControlsFlipped;/* || (acos(std::min<float>(1.0f, std::max<float>(-1.0f, 
            Vector2D::Dot(this->GetGameSpacePaddleUpUnitVec(), Vector2D(0,-1))))) <= M_PI_DIV4); */
	}
    bool AreControlsFlippedForOther() const {
        return this->areControlsFlipped;
    }

    Vector2D GetGameSpacePaddleRightUnitVec() const;
    Vector2D GetGameSpacePaddleUpUnitVec() const;

	// *******************************************************************

	void AddPossibleItemDrop(const LevelPiece& p);
	void AddItemDrop(const Point2D& p, const GameItem::ItemType& itemType);
	void AddProjectile(Projectile* projectile);
	void AddBeam(Beam* beam);

	bool AddStatusUpdateLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status);
	bool RemoveStatusForLevelPiece(LevelPiece* p, const LevelPiece::PieceStatus& status);

    void PerformLevelCompletionChecks();

	// Debug functions
#ifdef _DEBUG
	void DropItem(GameItem::ItemType itemType);
    void DropThreeItems(GameItem::ItemType itemType1, GameItem::ItemType itemType2, GameItem::ItemType itemType3);
#endif

private:
    GameState* currState;		// The current game state
    GameState* nextState;

    GameSound* sound;

    // Player-controllable game assets
    PlayerPaddle* playerPaddle;                         // The one and only player paddle
    std::list<GameBall*> balls;                         // Current set of balls active in the game
    SafetyNet* bottomSafetyNet;                         // The bottom (of the level) ball safety net
    SafetyNet* topSafetyNet;                            // The top (of the level) ball safety net
    std::list<Beam*> beams;                             // Beams spawned as the game is played
    std::map<LevelPiece*, int32_t> statusUpdatePieces;  // Pieces that require updating every frame due to status effects
    BallBoostModel* boostModel;                         // This is only not NULL when the ball is in play - it contains the model/state for ball boosting

    ProjectileMap projectiles;  // Projectiles spawned as the game is played

    // Current world and level information
    unsigned int currWorldNum;
    std::vector<GameWorld*> worlds;

    // Difficulty setting of the game
    GameModel::Difficulty difficulty;

    // Set Boost mode
    BallBoostModel::BallBoostMode ballBoostMode;

    // Items that are currently available to be picked up in-game
    std::list<GameItem*> currLiveItems;
    // Timers that are currently active
    std::list<GameItemTimer*> activeTimers;

    // Player score and life information
    long currPlayerScore;
    int numStarsAwarded;
    int numInterimBlocksDestroyed;
    int maxInterimBlocksDestroyed;
    int numLivesLostInLevel;
    bool droppedLifeForMaxMultiplier;

    int currLivesLeft;        // Number of lives left before the player is officially dead
    int livesAtStartOfLevel;  // Lives given to the player at the start of a level
    int maxNumLivesAllowed;   // The maximum container for lives allowed

    int numGoodItemsAcquired, numNeutralItemsAcquired, numBadItemsAcquired;
    double totalLevelTimeInSeconds;

    // Holds the type of pausing that is currently being used on the game model (see PauseType enum)
    int pauseBitField;

    bool gameIsEntirelyPaused;
    bool gameStateIsPaused;

    bool ballBoostIsInverted;

    // Item related tracking variables *******************
    bool areControlsFlipped;
    bool isBlackoutActive;										// Is the game currently blacked out?
    GameTransformMgr* gameTransformInfo;		// The current transform information of the game

    // ***************************************************

    bool doingPieceStatusListIteration;
    bool progressLoadedSuccessfully;

    // Private getters and setters ****************************************
    void SetCurrentWorldAndLevel(int worldIdx, int levelIdx, bool sendNewWorldEvent);

    void SetCurrentStateImmediately(GameState* newState) {
        assert(newState != NULL);
        if (this->currState != NULL) {
            delete this->currState;
        }
        this->currState = newState;
    }

    void CollisionOccurred(Projectile* projectile, LevelPiece* p);
    void CollisionOccurred(GameBall& ball, LevelPiece* p);
    void CollisionOccurred(GameBall& ball, Boss* boss, BossBodyPart* bossPart);
    void CollisionOccurred(Projectile* projectile, Boss* boss, BossBodyPart* bossPart);
    void CollisionOccurred(Projectile* projectile, PlayerPaddle* paddle);
    void CollisionOccurred(Projectile* projectile, GameBall* ball);

    void BallPaddleCollisionOccurred(GameBall& ball);
    void BallDied(GameBall* deadBall, bool& stateChanged);
    void DoPieceStatusUpdates(double dT);
    void DoProjectileCollisions(double dT);

    void UpdateActiveTimers(double seconds);
    void UpdateActiveItemDrops(double seconds);
    void UpdateActiveProjectiles(double seconds);
    void UpdateActiveBeams(double seconds);

    void ResetScore();
    void SetNumInterimBlocksDestroyed(int value, const Point2D& pos = Point2D());
    int  GetNumInterimBlocksDestroyed() const { return this->numInterimBlocksDestroyed; }
    void SetMaxConsecutiveBlocksDestroyed(int value) { this->maxInterimBlocksDestroyed = value; }
    int  GetCurrentMultiplier() const;


    void ResetLevelTime() { this->totalLevelTimeInSeconds = 0; }
    void ResetLivesLostCounter() { this->numLivesLostInLevel = 0; }

    void ResetNumAcquiredItems() {
        this->numGoodItemsAcquired = this->numNeutralItemsAcquired = this->numBadItemsAcquired = 0;
    }
    void IncrementNumAcquiredItems(const GameItem* item);

    void ClearProjectiles();
    void ClearBeams();
    void ClearLiveItems();
    void ClearActiveTimers();
    void ClearStatusUpdatePieces();
    void ClearBallsToOne();

    bool RemoveActiveGameItemsOfGivenType(const GameItem::ItemType& type);
    void RemoveActiveGameItemsForThisBallOnly(const GameBall* ball);

    void RemoveBallDeathMineProjectiles();

    void ResetLevelValues(int numLives);

    void LoadWorldsFromFile();

    PaddleRemoteControlRocketProjectile* GetActiveRemoteControlRocket() const;
};

inline void GameModel::ResetScore() {
    // Reset the actual score
    this->currPlayerScore = GameModelConstants::GetInstance()->INIT_SCORE;
    // EVENT: Score was changed
    GameEventManager::Instance()->ActionScoreChanged(this->currPlayerScore);

    // Reset the number of stars
    int oldNumStars = this->numStarsAwarded;
    this->numStarsAwarded = 0;
    // EVENT: Number of stars changed
    GameEventManager::Instance()->ActionNumStarsChanged(NULL, oldNumStars, this->numStarsAwarded);
}

inline void GameModel::ClearStatusUpdatePieces() {
	assert(!this->doingPieceStatusListIteration);
	// When we clear all the update pieces we also clear all the relevant status effects for those pieces...
	this->doingPieceStatusListIteration = true;
	for (std::map<LevelPiece*, int32_t>::iterator iter = this->statusUpdatePieces.begin(); iter != this->statusUpdatePieces.end(); ++iter) {
		LevelPiece* currLevelPiece = iter->first;
        currLevelPiece->RemoveStatuses(this->GetCurrentLevel(), iter->second);
	}
	this->statusUpdatePieces.clear();
	this->doingPieceStatusListIteration = false;
}

inline bool GameModel::IsOutOfGameBoundsForBall(const Point2D& pos) const {
    return this->IsOutOfPaddedLevelBounds(pos, GameLevel::OUT_OF_BOUNDS_X_BUFFER_SPACE_FOR_BALL, 
        GameLevel::OUT_OF_BOUNDS_Y_BUFFER_SPACE_FOR_BALL);
}

inline bool GameModel::IsOutOfGameBoundsForItem(const Point2D& pos) const {
    return this->IsOutOfPaddedLevelBounds(pos, GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_ITEM);
}

inline bool GameModel::IsOutOfGameBoundsForProjectile(const Point2D& pos) const {
    return this->IsOutOfPaddedLevelBounds(pos, GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE);
}

inline bool GameModel::IsOutOfPaddedLevelBounds(const Point2D& pos, float padding) const {
    return this->IsOutOfPaddedLevelBounds(pos, padding, padding);
}

inline bool GameModel::IsOutOfPaddedLevelBounds(const Point2D& pos, float xPadding, float yPadding) const {
    return this->IsOutOfPaddedLevelBounds(pos, xPadding, yPadding, xPadding, yPadding);
}

inline Collision::AABB2D GameModel::GenerateLevelProjectileBoundaries() const {
    const GameLevel* currLevel = this->GetCurrentLevel();
    return Collision::AABB2D(
        Point2D(-GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE, -GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE), 
        Point2D(currLevel->GetLevelUnitWidth()  + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE, 
        currLevel->GetLevelUnitHeight() + GameLevel::OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE));
}

#endif