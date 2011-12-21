/**
 * GameLevel.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GAMELEVEL_H__
#define __GAMELEVEL_H__

#include "LevelPiece.h"
#include "Projectile.h"
#include "GameItem.h"

#include <string>
#include <vector>
#include <set>

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
	static const char COLLATERAL_BLOCK_CHAR;
	static const char TESLA_BLOCK_CHAR;
	static const char ITEM_DROP_BLOCK_CHAR;
    static const char SWITCH_BLOCK_CHAR;
    static const char ONE_WAY_BLOCK_CHAR;
    static const char NO_ENTRY_BLOCK_CHAR;
    static const char LASER_TURRET_BLOCK_CHAR;
    static const char ROCKET_TURRET_BLOCK_CHAR;
	
	static const char TRIANGLE_BLOCK_CHAR;
	static const char TRI_UPPER_CORNER;
	static const char TRI_LOWER_CORNER;
	static const char TRI_LEFT_CORNER;
	static const char TRI_RIGHT_CORNER;

	static const char* ALL_ITEM_TYPES_KEYWORD;
	static const char* POWERUP_ITEM_TYPES_KEYWORD;
	static const char* POWERNEUTRAL_ITEM_TYPES_KEYWORD;
	static const char* POWERDOWN_ITEM_TYPES_KEYWORD;

    static const char* STAR_POINT_MILESTONE_KEYWORD;

	static const int OUT_OF_BOUNDS_BUFFER_SPACE = 9;
	static const int Y_COORD_OF_DEATH = -OUT_OF_BOUNDS_BUFFER_SPACE;

	~GameLevel();

	// Used to create a level from file
	static GameLevel* CreateGameLevelFromFile(size_t levelNumber, std::string filepath);

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

	std::set<LevelPiece*> GetLevelPieceCollisionCandidates(const Point2D& center, float radius) const;
	std::set<LevelPiece*> GetLevelPieceCollisionCandidates(const Projectile& p) const;
	std::set<LevelPiece*> GetLevelPieceCollisionCandidates(const PlayerPaddle& p, bool includeAttachedBall) const;

	LevelPiece* GetLevelPieceFirstCollider(const Collision::Ray2D& ray, 
        const std::set<const LevelPiece*>& ignorePieces, float& rayT, float toleranceRadius = 0.0f) const;
    void GetLevelPieceColliders(const Collision::Ray2D& ray, const std::set<const LevelPiece*>& ignorePieces,
        const std::set<LevelPiece::LevelPieceType>& ignorePieceTypes, std::list<LevelPiece*>& result, float toleranceRadius = 0.0f) const;

	// Get whether or not the ball safety net is currently active
	bool IsBallSafetyNetActive() const {
		return this->ballSafetyNetActive;
	}
	/**
	 * Toggle whether the ball safety net is on or not.
	 */
	void ToggleBallSafetyNet(bool isActive) {
		this->ballSafetyNetActive = isActive;
	}
	bool BallSafetyNetCollisionCheck(const GameBall& b, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
	bool PaddleSafetyNetCollisionCheck(const PlayerPaddle& p);
	bool ProjectileSafetyNetCollisionCheck(const Projectile& p, const BoundingLines& projectileBoundingLines);

	// Ability to add/remove tesla lightning barriers
	void AddTeslaLightningBarrier(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2);
	void RemoveTeslaLightningBarrier(const TeslaBlock* block1, const TeslaBlock* block2);
	bool TeslaLightningCollisionCheck(const GameBall& b, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
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

	float GetPaddleMinBound() const;
	float GetPaddleMaxBound() const;

    LevelPiece* GetMinPaddleBoundPiece() const;
    LevelPiece* GetMaxPaddleBoundPiece() const;

	float GetLevelUnitWidth() const {
		return this->width * LevelPiece::PIECE_WIDTH;
	}

	float GetLevelUnitHeight() const {
		return this->height * LevelPiece::PIECE_HEIGHT;
	}

	bool IsLevelComplete() const {
		return this->piecesLeft == 0;
	}

	const std::string& GetFilepath() const {
		return this->filepath;
	}
	const std::string& GetName() const {
		return this->levelName;
	}

    // Get the zero-based level number/index in its world
    size_t GetLevelNumIndex() const { return this->levelNum; }

	void PieceChanged(GameModel* gameModel, LevelPiece* pieceBefore, LevelPiece* pieceAfter,
                      const LevelPiece::DestructionMethod& method);
	LevelPiece* RocketExplosion(GameModel* gameModel, const RocketProjectile* rocket, LevelPiece* hitPiece);
    void RocketExplosionNoPieces(const Projectile* rocket);
	std::vector<LevelPiece*> GetRocketExplosionAffectedLevelPieces(float rocketSizeFactor, size_t hIndex, size_t wIndex);

    void ActivateTriggerableLevelPiece(const LevelPiece::TriggerID& triggerID, GameModel* gameModel);
    const LevelPiece* GetTriggerableLevelPiece(const LevelPiece::TriggerID& triggerID) const;

    long GetHighScore() const;
    void SetHighScore(long highScore);
    bool GetHasNewHighScore() const;
    void SetNewHighScore(bool newHighScore);

    int GetNumStarsForScore(long score) const;
    int GetHighScoreNumStars() const { return this->GetNumStarsForScore(this->highScore); }

    void TickAIEntities(double dT, GameModel* gameModel);

private:
	// Map of the pairings of tesla blocks and their active lightning arc that enforces bounds
	// on the level as long as it's active
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D> teslaLightning;

	std::vector<std::vector<LevelPiece*> > currentLevelPieces; // The current layout of the level, stored in row major format
    std::map<LevelPiece::TriggerID, LevelPiece*> triggerablePieces;

    std::set<LevelPiece*> aiEntities;

	BoundingLines safetyNetBounds;
	std::string filepath;
	std::string levelName;

    size_t levelNum;                    // The zero-based index of this level in its world
	size_t piecesLeft;                  // Pieces left before the end of the level
	size_t width, height;               // Size values for the level
    size_t randomItemProbabilityNum;    // A number >= 0 for random item probability in the level

    std::vector<GameItem::ItemType> allowedDropTypes;	// The random allowed drop types that come from destroyed blocks in this level

    // Persistant scoring variables - used to mark previously saved scores and calculate high scores
    long starAwardScores[5];   // Scores where stars are awarded
    long highScore;            // Current high score for this level

    bool newHighScore;        // If a new high score was achieved on the last play through of this level
    bool ballSafetyNetActive;
	//bool isLocked;    // Whether this level has been unlocked or not

	GameLevel(size_t levelNumber, const std::string& filepath, const std::string& levelName, unsigned int numBlocks, 
		const std::vector<std::vector<LevelPiece*> >& pieces, const std::vector<GameItem::ItemType>& allowedDropTypes,
        size_t randomItemProbabilityNum, long* starAwardScores);
	
	static void UpdatePiece(const std::vector<std::vector<LevelPiece*> >& pieces, size_t hIndex, size_t wIndex);
	std::set<LevelPiece*> IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax) const;
	static void CleanUpFileReadData(std::vector<std::vector<LevelPiece*> >& levelPieces);

    DISALLOW_COPY_AND_ASSIGN(GameLevel);
};

inline const LevelPiece* GameLevel::GetTriggerableLevelPiece(const LevelPiece::TriggerID& triggerID) const {
    std::map<LevelPiece::TriggerID, LevelPiece*>::const_iterator findIter = this->triggerablePieces.find(triggerID);
    if (findIter == this->triggerablePieces.end()) {
        return NULL;
    }
    return findIter->second;
}

inline long GameLevel::GetHighScore() const {
    return this->highScore;
}

inline void GameLevel::SetHighScore(long highScore) {
    this->highScore = highScore;
    // TODO: Save game progress to file
}

// Get whether this level has a new high score due to its most recent play-through this game
inline bool GameLevel::GetHasNewHighScore() const {
    return this->newHighScore;
}
inline void GameLevel::SetNewHighScore(bool newHighScore) {
    this->newHighScore = newHighScore;
}

inline int GameLevel::GetNumStarsForScore(long score) const {
    int starIdx = 0;
    while (score >= this->starAwardScores[starIdx] && starIdx < MAX_STARS_PER_LEVEL) {
        starIdx++;
    }
    return starIdx;
}

inline LevelPiece* GameLevel::GetMinPaddleBoundPiece() const {
    return this->currentLevelPieces[0][0];
}
inline LevelPiece* GameLevel::GetMaxPaddleBoundPiece() const {
    return this->currentLevelPieces[0][this->width-1];
}

#endif

