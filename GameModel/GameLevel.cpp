#include "GameLevel.h"
#include "LevelPiece.h"
#include "../Utils/Debug.h"

#include <iostream>
#include <fstream>

// Private constructor, requires all the pieces that make up the level
GameLevel::GameLevel(unsigned int numBlocks, std::vector<std::vector<LevelPiece*>> pieces): cachedInitialLevelPieces(pieces), currentLevelPieces(pieces),
piecesLeft(numBlocks) {
	assert(pieces.size() > 0);
	this->width = pieces[0].size();
	this->height = pieces.size();
}

// Destructor, clean up heap stuffs
GameLevel::~GameLevel() {
	for (size_t i = 0; i < this->cachedInitialLevelPieces.size(); i++) {
		for (size_t j = 0; j < this->cachedInitialLevelPieces[i].size(); j++) {
			delete this->cachedInitialLevelPieces[i][j];
		}
		this->cachedInitialLevelPieces[i].clear();
	}
	this->cachedInitialLevelPieces.clear();

	for (size_t i = 0; i < this->currentLevelPieces.size(); i++) {
		for (size_t j = 0; j < this->currentLevelPieces[i].size(); j++) {
			delete this->currentLevelPieces[i][j];
		}
		this->currentLevelPieces[i].clear();
	}
	this->currentLevelPieces.clear();
}

GameLevel* GameLevel::CreateGameLevelFromFile(std::string filepath) {
	std::ifstream inFile;
	inFile.open(filepath.c_str());
	
	// Make sure the file opened properly
	if (!inFile.is_open()) {
		debug_output("ERROR: Could not open level file: " << filepath); 
		return NULL;
	}

	// Read in the file width and height
	int width = 0;
	int height = 0;
	if (!(inFile >> width && inFile >> height)) {
		inFile.close();
		debug_output("ERROR: Error reading in width/height for file: " << filepath); 
		return NULL;
	}
	
	// Ensure width and height are valid
	if (width <= 0 || height <= 0) {
		inFile.close();
		debug_output("ERROR: Invalid width/height values for file: " << filepath); 
		return NULL;	
	}

	std::vector<std::vector<LevelPiece*>> levelPieces;
	unsigned int numVitalPieces = 0;

	// Read in the values that make up the level
	for (int h = 0; h < height; h++) {
		std::vector<LevelPiece*> currentRowPieces;

		for (int w = 0; w < width; w++) {
			char currBlock;
			
			// Read in the current level piece / block
			if (!(inFile >> currBlock)) {
					inFile.close();
					debug_output("ERROR: Could not properly read level interior value at width = " << w << ", height = " << h); 
					return NULL;	
			}

			// Validate the type of block
			if (!LevelPiece::IsValidBlockEnum(currBlock)) {
					inFile.close();
					debug_output("ERROR: Invalid level interior value: " << currBlock << " at width = " << w << ", height = " << h); 
					return NULL;	
			}
		
			// Create the level piece
			LevelPiece* newPiece = new LevelPiece(static_cast<unsigned int>(w), 
																					 static_cast<unsigned int>(height - 1 - h), 
																					 static_cast<LevelPiece::LevelPieceType>(currBlock));
			currentRowPieces.push_back(newPiece);
	
			if (newPiece->MustBeDestoryedToEndLevel()) {
				numVitalPieces++;
			}
		}

		levelPieces.insert(levelPieces.begin(), currentRowPieces);
	}

	inFile.close();

	// Go through all the pieces and initialize their bounding values appropriately
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			GameLevel::UpdatePiece(levelPieces, h, w);
		}
	}

	return new GameLevel(numVitalPieces, levelPieces);
}

/**
 * Updates the level piece at the given index.
 */
void GameLevel::UpdatePiece(const std::vector<std::vector<LevelPiece*>>& pieces, size_t hIndex, size_t wIndex) {
	
	// Make the provided indices are in the correct range
	if (wIndex < 0 || wIndex >= pieces[0].size() || hIndex <0 || hIndex >= pieces.size()) {
		return;
	}
	
	LevelPiece* leftNeighbor = NULL;
	LevelPiece* bottomNeighbor = NULL;
	LevelPiece* rightNeighbor = NULL;
	LevelPiece* topNeighbor = NULL;

	if (wIndex != 0) {
		leftNeighbor = pieces[hIndex][wIndex-1];
	}
	if (wIndex != pieces[wIndex].size()-1) {
		rightNeighbor = pieces[hIndex][wIndex+1];
	}
	if (hIndex != 0) {
		bottomNeighbor = pieces[hIndex-1][wIndex];
	}
	if (hIndex != pieces.size()-1) {
		topNeighbor = pieces[hIndex+1][wIndex];
	}	
	pieces[hIndex][wIndex]->UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

/**
 * Call when a collision occurs with the ball and the level object at
 * the given index.
 */
void GameLevel::BallCollisionOccurred(size_t hIndex, size_t wIndex) {
	assert(hIndex >= 0 && hIndex < this->height);
	assert(wIndex >= 0 && wIndex < this->width);

	// Update the current piece by executing the collision
	LevelPiece* currPiece = this->currentLevelPieces[hIndex][wIndex];
	bool isVitalBeforeCollision = currPiece->MustBeDestoryedToEndLevel();
	currPiece->BallCollisionOccurred();
	bool isVitalAfterCollision = currPiece->MustBeDestoryedToEndLevel();

	// Update the number of pieces that need to be destoryed to end the level
	if (isVitalBeforeCollision && !isVitalAfterCollision) {
		// In this case a block that was vital has been destroyed
		this->piecesLeft--;
		assert(this->piecesLeft >= 0);
	}

	// Update the neighbour's bounds...
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex-1); // left
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex); // bottom
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex+1); // right
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex); // top
}