#ifndef __GAMELEVEL_H__
#define __GAMELEVEL_H__

#include "LevelPiece.h"

#include <string>
#include <vector>

// Represents a game level, also deals with game level 'lvl' file reading.
class GameLevel {

private:	
	// The initial layout of the level, stored in row major format (i.e., [row][col] or [height][width])
	std::vector<std::vector<LevelPiece*>> cachedInitialLevelPieces;
	// The current layout of the level, stored in row major format
	std::vector<std::vector<LevelPiece*>> currentLevelPieces;
	// Pieces left before the end of the level
	unsigned int piecesLeft;
	// Size values for the level
	unsigned int width, height;

	GameLevel(unsigned int numBlocks, std::vector<std::vector<LevelPiece*>> pieces);
	
	static void UpdatePiece(const std::vector<std::vector<LevelPiece*>>& pieces, size_t hIndex, size_t wIndex);

public:
	static const int Y_COORD_OF_DEATH = -5;

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

	float GetPaddleMinBound() const {
		LevelPiece* temp = this->cachedInitialLevelPieces[0][0];
		return temp->GetCenter()[0] + temp->GetHalfWidth();
	}

	float GetPaddleMaxBound() const {
		LevelPiece* temp = this->cachedInitialLevelPieces[0][this->width-1];
		return temp->GetCenter()[0] - temp->GetHalfWidth();	
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

	void BallCollisionOccurred(size_t hIndex, size_t wIndex);


};
#endif

