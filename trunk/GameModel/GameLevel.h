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
	
	static const char TRIANGLE_BLOCK_CHAR;
	static const char TRI_UPPER_CORNER;
	static const char TRI_LOWER_CORNER;
	static const char TRI_LEFT_CORNER;
	static const char TRI_RIGHT_CORNER;

	static const char* ALL_ITEM_TYPES_KEYWORD;
	static const char* POWERUP_ITEM_TYPES_KEYWORD;
	static const char* POWERNEUTRAL_ITEM_TYPES_KEYWORD;
	static const char* POWERDOWN_ITEM_TYPES_KEYWORD;

	static const int OUT_OF_BOUNDS_BUFFER_SPACE = 5;
	static const int Y_COORD_OF_DEATH = -OUT_OF_BOUNDS_BUFFER_SPACE;

	~GameLevel();

	// Used to create a level from file
	static GameLevel* CreateGameLevelFromFile(std::string filepath);

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

	LevelPiece* GetLevelPieceFirstCollider(const Collision::Ray2D& ray, std::set<const LevelPiece*> ignorePieces, float& rayT, float toleranceRadius = 0.0f) const;
	
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

	float GetPaddleMinBound() const {
		LevelPiece* temp = this->currentLevelPieces[0][0];
		return temp->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
	}

	float GetPaddleMaxBound() const {
		LevelPiece* temp = this->currentLevelPieces[0][this->width-1];
		return temp->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;	
	}

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

	void PieceChanged(GameModel* gameModel, LevelPiece* pieceBefore, LevelPiece* pieceAfter);
	LevelPiece* RocketExplosion(GameModel* gameModel, const Projectile* rocket, LevelPiece* hitPiece);
	std::vector<LevelPiece*> GetRocketExplosionAffectedLevelPieces(float rocketSizeFactor, size_t hIndex, size_t wIndex);

private:	
	std::vector<std::vector<LevelPiece*> > currentLevelPieces; // The current layout of the level, stored in row major format
    //std::map<LevelPiece::TriggerID, LevelPiece*> triggerablePieces;

	unsigned int piecesLeft;            // Pieces left before the end of the level
	unsigned int width, height;         // Size values for the level
	bool ballSafetyNetActive;
	BoundingLines safetyNetBounds;
	std::string filepath;
	std::string levelName;

	std::vector<GameItem::ItemType> allowedDropTypes;	// The random allowed drop types that come from destroyed blocks in this level
	size_t randomItemProbabilityNum;									// A number >= 0 for random item probability in the level

	// Map of the pairings of tesla blocks and their active lightning arc that enforces bounds
	// on the level as long as it's active
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D> teslaLightning;

	//QuadTree* levelTree;	// A quad tree representing the boundries of this entire level and all its pieces

	GameLevel(const std::string& filepath, const std::string& levelName, unsigned int numBlocks, 
		const std::vector<std::vector<LevelPiece*> >& pieces, const std::vector<GameItem::ItemType>& allowedDropTypes, size_t randomItemProbabilityNum);
	
	static void UpdatePiece(const std::vector<std::vector<LevelPiece*> >& pieces, size_t hIndex, size_t wIndex);
	std::set<LevelPiece*> IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax) const;


	static void CleanUpFileReadData(std::vector<std::vector<LevelPiece*> >& levelPieces);

};
#endif

