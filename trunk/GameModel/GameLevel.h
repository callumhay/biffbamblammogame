#ifndef __GAMELEVEL_H__
#define __GAMELEVEL_H__

#include "LevelPiece.h"
#include "Projectile.h"

#include <string>
#include <vector>
#include <set>

class GameBall;

// Represents a game level, also deals with game level 'lvl' file reading.
class GameLevel {

private:	
	static const char EMPTY_SPACE_CHAR;
	static const char SOLID_BLOCK_CHAR;
	static const char GREEN_BREAKABLE_CHAR;
	static const char YELLOW_BREAKABLE_CHAR;
	static const char ORANGE_BREAKABLE_CHAR;
	static const char RED_BREAKABLE_CHAR;
	static const char BOMB_CHAR;

	// The current layout of the level, stored in row major format
	std::vector<std::vector<LevelPiece*>> currentLevelPieces;
	// Pieces left before the end of the level
	unsigned int piecesLeft;
	// Size values for the level
	unsigned int width, height;

	GameLevel(unsigned int numBlocks, std::vector<std::vector<LevelPiece*>> pieces);
	
	static void UpdatePiece(const std::vector<std::vector<LevelPiece*>>& pieces, size_t hIndex, size_t wIndex);
	std::set<LevelPiece*> IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax) const;

public:
	static const int OUT_OF_BOUNDS_BUFFER_SPACE = 5;
	static const int Y_COORD_OF_DEATH = -OUT_OF_BOUNDS_BUFFER_SPACE;

	~GameLevel();

	// Used to create a level from file
	static GameLevel* CreateGameLevelFromFile(std::string filepath);

	/**
	 * Obtain the set of pieces making up the current state of the level.
	 * Return: Reference to the row major, 2D array of level pieces as they currently stand.
	 */
	const std::vector<std::vector<LevelPiece*>>& GetCurrentLevelLayout() const {
		return this->currentLevelPieces;
	}

	std::set<LevelPiece*> GetCollisionCandidates(const GameBall& b) const;
	std::set<LevelPiece*> GetCollisionCandidates(const Projectile& p) const;

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

	void PieceChanged(LevelPiece* pieceBefore, LevelPiece* pieceAfter);
};
#endif
