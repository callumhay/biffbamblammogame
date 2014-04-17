/**
 * GameLevel.h
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

#ifndef __GAMELEVEL_H__
#define __GAMELEVEL_H__

#include "LevelPiece.h"
#include "Projectile.h"
#include "GameItem.h"
#include "GameWorld.h"
#include "Boss.h"

#include <string>
#include <vector>
#include <set>

class GameSound;
class GameBall;
class QuadTree;
class TeslaBlock;
class PlayerPaddle;

// Represents a game level, also deals with game level 'lvl' file reading.
class GameLevel {
public:
    static const int MAX_STARS_PER_LEVEL;

	static const char EMPTY_SPACE_CHAR;
	static const char SOLID_BLOCK_CHAR;
	static const char GREEN_BREAKABLE_CHAR;
	static const char YELLOW_BREAKABLE_CHAR;
	static const char ORANGE_BREAKABLE_CHAR;
	static const char RED_BREAKABLE_CHAR;
	static const char BOMB_CHAR;
	static const char INKBLOCK_CHAR;
	static const char PRISM_BLOCK_CHAR;
	static const char PORTAL_BLOCK_CHAR;
	static const char CANNON_BLOCK_CHAR;
    static const char ONE_SHOT_CANNON_BLOCK_QUALIFIER_CHAR;
	static const char COLLATERAL_BLOCK_CHAR;
	static const char TESLA_BLOCK_CHAR;
	static const char ITEM_DROP_BLOCK_CHAR;
    static const char SWITCH_BLOCK_CHAR;
    static const char ONE_WAY_BLOCK_CHAR;
    static const char NO_ENTRY_BLOCK_CHAR;
    static const char LASER_TURRET_BLOCK_CHAR;
    static const char ROCKET_TURRET_BLOCK_CHAR;
    static const char MINE_TURRET_BLOCK_CHAR;
    static const char ALWAYS_DROP_BLOCK_CHAR;
    static const char REGEN_BLOCK_CHAR;
	
	static const char TRIANGLE_BLOCK_CHAR;
	static const char TRI_UPPER_CORNER;
	static const char TRI_LOWER_CORNER;
	static const char TRI_LEFT_CORNER;
	static const char TRI_RIGHT_CORNER;

    static const char FINITE_LIFE_CHAR;
    static const char INFINITE_LIFE_CHAR;

    static const char* BOSS_LEVEL_KEYWORD;

	static const char* ALL_ITEM_TYPES_KEYWORD;
	static const char* POWERUP_ITEM_TYPES_KEYWORD;
	static const char* POWERNEUTRAL_ITEM_TYPES_KEYWORD;
	static const char* POWERDOWN_ITEM_TYPES_KEYWORD;

    static const char* STAR_POINT_MILESTONE_KEYWORD;

    static const char* PADDLE_STARTING_X_POS;
    static const int MIN_Y_BOUND_BUFFER_SPACE_FOR_PADDLE = 8;
    static const int DEFAULT_PADDLE_START_IDX = -1;

    static const int OUT_OF_BOUNDS_X_BUFFER_SPACE_FOR_BALL = 16;
	static const int OUT_OF_BOUNDS_Y_BUFFER_SPACE_FOR_BALL = 10;
    static const int OUT_OF_BOUNDS_BUFFER_SPACE_FOR_ITEM = 8;
    static const int OUT_OF_BOUNDS_BUFFER_SPACE_FOR_PROJECTILE = 13;
	static const int Y_COORD_OF_DEATH = -OUT_OF_BOUNDS_Y_BUFFER_SPACE_FOR_BALL;

    static const int NUM_PIECES_FOR_ALMOST_COMPLETE = 3;

	~GameLevel();

	// Used to create a level from file
	static GameLevel* CreateGameLevelFromFile(GameModel* gameModel, const GameWorld::WorldStyle& style, 
        size_t levelIdx, int milestoneStarAmt, std::string filepath);
    static bool ReadItemList(std::stringstream& inFile, std::vector<GameItem::ItemType>& items);

	/**
	 * Obtain the set of pieces making up the current state of the level.
	 * Return: Reference to the row major, 2D array of level pieces as they currently stand.
	 */
	const std::vector<std::vector<LevelPiece*> >& GetCurrentLevelLayout() const {
		return this->currentLevelPieces;
	}

	const std::vector<GameItem::ItemType>& GetAllowableItemDropTypes() const {
		return this->allowedDropTypes;
	}
	size_t GetRandomItemDropProbabilityNum() const {
		return this->randomItemProbabilityNum;
	}

    bool IsCollidingWithLevelPieces(const Point2D& center, float radius) const;

    void GetLevelPieceCollisionCandidatesNotMoving(const Point2D& center, float radius, std::vector<LevelPiece*>& candidates) const;
	void GetLevelPieceCollisionCandidates(double dT, const Point2D& center, float radius, float velocityMagnitude, std::vector<LevelPiece*>& candidates) const;
    void GetLevelPieceCollisionCandidates(const Collision::AABB2D& aabb, std::set<LevelPiece*>& candidates) const;
    void GetLevelPieceCollisionCandidatesNoSort(const Point2D& center, float radius, std::set<LevelPiece*>& candidates) const;
	void GetLevelPieceCollisionCandidates(double dT, const Point2D& center, const BoundingLines& bounds, float velocityMagnitude, std::set<LevelPiece*>& candidates) const;
	void GetLevelPieceCollisionCandidates(const PlayerPaddle& p, bool includeAttachedBall, std::set<LevelPiece*>& candidates) const;
    void GetLevelPieceCollisionCandidatesNotMoving(const Point2D& center, float width, float height, std::set<LevelPiece*>& candidates) const;

    static void BuildCollisionBoundsCombinationAndMap(const std::vector<LevelPiece*>& pieces,
        std::map<size_t, LevelPiece*>& boundsIdxMap, BoundingLines& combinationBounds);

    LevelPiece* GetLevelPieceAt(const Point2D& p) const;
    LevelPiece* GetLevelPieceColliderFast(const Collision::Ray2D& ray, float toleranceRadius = 0.0f) const;
	LevelPiece* GetLevelPieceFirstCollider(const Collision::Ray2D& ray, 
        const std::set<const void*>& ignoreThings, float& rayT, float toleranceRadius = 0.0f) const;
    void GetLevelPieceColliders(const Collision::Ray2D& ray, const std::set<const void*>& ignoreThings,
        const std::set<LevelPiece::LevelPieceType>& ignorePieceTypes, std::set<LevelPiece*>& result, 
        float cutoffRayT, float toleranceRadius = 0.0f) const;
    
	// Ability to add/remove Tesla lightning barriers
	void AddTeslaLightningBarrier(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2);
	void RemoveTeslaLightningBarrier(const TeslaBlock* block1, const TeslaBlock* block2);
	bool TeslaLightningCollisionCheck(const GameBall& b, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, 
        double& timeUntilCollision, Point2D& pointOfCollision) const;
	bool TeslaLightningCollisionCheck(const BoundingLines& bounds) const;
    bool IsDestroyedByTelsaLightning(const Projectile& p) const;
    void InitAfterLevelLoad(GameModel* model);

	/**
	 * Obtain the LevelPiece at the given height and width indices.
	 * Return: Pointer to LevelPiece at (h,w), NULL if no such index exists.
	 */
	LevelPiece* GetLevelPieceFromCurrentLayout(unsigned int hIndex, unsigned int wIndex) const {
		// Make sure the provided indices are in the correct range
		if (wIndex < 0 || wIndex >= this->currentLevelPieces[0].size() || hIndex <0 || hIndex >= this->currentLevelPieces.size()) {
			return NULL;
		}
		return this->currentLevelPieces[hIndex][wIndex];
	}

	float GetPaddleMinXBound(float paddleYPos) const;
	float GetPaddleMaxXBound(float paddleYPos) const;

    float GetPaddleMinXBound(float paddleStartingXPos, float paddleStartingYPos) const;
    float GetPaddleMaxXBound(float paddleStartingXPos, float paddleStartingYPos) const;

    float GetPaddleStartingXPosition() const {
        return this->paddleStartXPos;
    }

    LevelPiece* GetMinXPaddleBoundPiece(float paddleYPos) const;
    LevelPiece* GetMaxXPaddleBoundPiece(float paddleYPos) const;

    bool IsPaddleBoundPiece(const LevelPiece* piece) const;
    LevelPiece* GetMinXPaddleBoundPiece(int startingRowIdx, int startingColIdx) const;
    LevelPiece* GetMaxXPaddleBoundPiece(int startingRowIdx, int startingColIdx) const;

	float GetLevelUnitWidth() const {
		return this->width * LevelPiece::PIECE_WIDTH;
	}

	float GetLevelUnitHeight() const {
		return this->height * LevelPiece::PIECE_HEIGHT;
	}

	bool IsLevelComplete() const {
        if (this->GetHasBoss()) {
            return this->boss->GetIsLevelCompleteDead();
        }
		return this->piecesLeft == 0;
	}
    bool IsLevelAlmostComplete() const {
        if (this->GetHasBoss()) {
            return false;
        }
        return this->piecesLeft <= GameLevel::NUM_PIECES_FOR_ALMOST_COMPLETE;
    }
    void SignalLevelAlmostCompleteEvent();

    size_t GetNumPiecesLeft() const {
        return this->piecesLeft;
    }

	const std::string& GetFilepath() const {
		return this->filepath;
	}
	const std::string& GetName() const {
		return this->levelName;
	}

    // Get the zero-based level number/index in its world
    size_t GetLevelIndex() const { return this->levelIdx; }

	void PieceChanged(GameModel* gameModel, LevelPiece* pieceBefore, LevelPiece* pieceAfter,
                      const LevelPiece::DestructionMethod& method);
    void UpdateBoundsOnPieceAndSurroundingPieces(LevelPiece* piece);
	
    LevelPiece* RocketExplosion(GameModel* gameModel, const RocketProjectile* rocket, LevelPiece* hitPiece);
    void RocketExplosionNoPieces(const RocketProjectile* rocket);
	std::set<LevelPiece*> GetExplosionAffectedLevelPieces(const Projectile* projectile, float sizeFactor, LevelPiece* centerPiece);

    LevelPiece* MineExplosion(GameModel* gameModel, const MineProjectile* mine, LevelPiece* hitPiece);
    void MineExplosion(GameModel* gameModel, const MineProjectile* mine);

    void ActivateTriggerableLevelPiece(const LevelPiece::TriggerID& triggerID, GameModel* gameModel);
    const std::vector<LevelPiece*>* GetTriggerableLevelPieces(const LevelPiece::TriggerID& triggerID) const;

    bool GetIsLevelPassedWithScore() const;
    long GetPrevHighScore() const;
 
    long GetHighScore() const;
    void SetHighScore(long highScore, bool setPrevAsWell);

    bool GetHasNewHighScore() const;
    void SetNewHighScore(bool hasNewHighScore);

    int GetNumStarsForScore(long score) const;
    int GetHighScoreNumStars() const { return this->GetNumStarsForScore(this->highScore); }

    // Get the number of stars required to unlock the level when it isn't already unlocked
    int GetNumStarsRequiredToUnlock() const;
    bool GetAreUnlockStarsPaidFor() const;
    void SetAreUnlockStarsPaidFor(bool paidFor);

    Boss* GetBoss() const {
        return this->boss;
    }
    bool GetHasBoss() const {
        return (this->boss != NULL);
    }
    bool CollideBossWithLevel(const Collision::AABB2D& bossAABB, Vector2D& correctionVec) const;

    void TickAIEntities(double dT, GameModel* gameModel);

    Vector3D GetTranslationToMiddle() const {
        return Vector3D(-0.5f * this->GetLevelUnitWidth(), -0.5f * this->GetLevelUnitHeight(), 0.0f);
    }
    Vector2D GetTranslationToMiddle2D() const {
        return Vector2D(-0.5f * this->GetLevelUnitWidth(), -0.5f * this->GetLevelUnitHeight());
    }

    void GetLevelAABB(Collision::AABB2D& levelAABB) const {
        levelAABB.SetMin(Point2D(0, 0));
        levelAABB.SetMax(Point2D(this->GetLevelUnitWidth(), this->GetLevelUnitHeight()));
    }

private:
	// Map of the pairings of Tesla blocks and their active lightning arc that enforces bounds
	// on the level as long as it's active
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D> teslaLightning;
    BoundingLines teslaLightningBounds;

    void RebuildTeslaLightningBoundingLines();

	std::vector<std::vector<LevelPiece*> > currentLevelPieces; // The current layout of the level, stored in row major format

    typedef std::map<LevelPiece::TriggerID, std::vector<LevelPiece*> > TriggerPiecesMap;
    typedef TriggerPiecesMap::const_iterator TriggerPiecesMapConstIter;
    typedef TriggerPiecesMap::iterator TriggerPiecesMapIter;

    TriggerPiecesMap triggerablePieces;

    std::set<LevelPiece*> aiEntities;

	std::string filepath;
	std::string levelName;

    size_t levelIdx;                    // The zero-based index of this level in its world
	size_t piecesLeft;                  // Pieces left before the end of the level
	size_t width, height;               // Size values for the level
    size_t randomItemProbabilityNum;    // A number >= 0 for random item probability in the level
    float paddleStartXPos;
    float levelHypotenuse;

    bool levelAlmostCompleteSignaled; // Whether or not the event for the level being almost completed has already been signaled

    std::vector<GameItem::ItemType> allowedDropTypes;	// The random allowed drop types that come from destroyed blocks in this level

    Boss* boss; // If the current level has a boss, this is a pointer to it, otherwise it will be NULL

    // Persistent scoring variables - used to mark previously saved scores and calculate high scores
    long starAwardScores[5];  // Scores where stars are awarded
    long prevHighScore;
    long highScore;           // Current high score for this level
    bool hasNewHighScore;     // If a new high score was achieved on the last play through of this level

    // Milestone star lock -- keeps players from entering or going past this level until the level is unlocked
    // via a given number of stars
    int numStarsRequiredToUnlock;
    bool areUnlockStarsPaidFor;

    // Constructor for non-boss levels
	GameLevel(size_t levelIdx, const std::string& filepath, const std::string& levelName, unsigned int numBlocks, 
		const std::vector<std::vector<LevelPiece*> >& pieces, int numStarsToUnlock,
        const std::vector<GameItem::ItemType>& allowedDropTypes,
        size_t randomItemProbabilityNum, long* starAwardScores, float paddleStartXPos);
	// Constructor for boss levels
    GameLevel(size_t levelIdx, const std::string& filepath, const std::string& levelName, 
		const std::vector<std::vector<LevelPiece*> >& pieces, int numStarsToUnlock,
        Boss* boss, const std::vector<GameItem::ItemType>& allowedDropTypes,
        size_t randomItemProbabilityNum, float paddleStartXPos);

    void InitPieces(float paddleStartXPos, const std::vector<std::vector<LevelPiece*> >& pieces);
    void SetPaddleStartXPos(float xPos);

	static void UpdatePiece(const std::vector<std::vector<LevelPiece*> >& pieces, size_t hIndex, size_t wIndex);
    void IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax, std::set<LevelPiece*>& candidates) const;

	static void CleanUpFileReadData(std::vector<std::vector<LevelPiece*> >& levelPieces);

    DISALLOW_COPY_AND_ASSIGN(GameLevel);
};

inline const std::vector<LevelPiece*>* GameLevel::GetTriggerableLevelPieces(const LevelPiece::TriggerID& triggerID) const {
    TriggerPiecesMapConstIter findIter = this->triggerablePieces.find(triggerID);
    if (findIter == this->triggerablePieces.end()) {
        return NULL;
    }
    return &findIter->second;
}

inline bool GameLevel::GetIsLevelPassedWithScore() const {
    return this->highScore > 0;
}
inline long GameLevel::GetPrevHighScore() const {
    return this->prevHighScore;
}

inline long GameLevel::GetHighScore() const {
    return this->highScore;
}

inline void GameLevel::SetHighScore(long highScore, bool setPrevAsWell) {
    if (setPrevAsWell) {
        this->prevHighScore = highScore;
    }
    else {
        this->prevHighScore = this->highScore;
    }
    this->highScore = highScore;
}

/**
 * Checks if the given bounding lines collide with any Tesla lightning arcs currently active in this level.
 */
inline bool GameLevel::TeslaLightningCollisionCheck(const BoundingLines& bounds) const {
    return this->teslaLightningBounds.CollisionCheck(bounds);
}

// Get whether this level has a new high score due to its most recent play-through this game
inline bool GameLevel::GetHasNewHighScore() const {
    return this->hasNewHighScore;
}
inline void GameLevel::SetNewHighScore(bool hasNewHighScore) {
    this->hasNewHighScore = hasNewHighScore;
}

inline int GameLevel::GetNumStarsForScore(long score) const {
    int starIdx = 0;
    while (score >= this->starAwardScores[starIdx] && starIdx < MAX_STARS_PER_LEVEL) {
        starIdx++;
    }
    return starIdx;
}

inline int GameLevel::GetNumStarsRequiredToUnlock() const {
    return this->numStarsRequiredToUnlock;
}
inline bool GameLevel::GetAreUnlockStarsPaidFor() const {
    return this->areUnlockStarsPaidFor || this->GetNumStarsRequiredToUnlock() == 0;
}
inline void GameLevel::SetAreUnlockStarsPaidFor(bool paidFor) {
    this->areUnlockStarsPaidFor = paidFor;
}

#endif

