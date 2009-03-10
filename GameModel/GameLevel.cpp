#include "../BlammoEngine/BlammoEngine.h"

#include "GameLevel.h"
#include "GameBall.h"

#include "GameEventManager.h"
#include "LevelPiece.h"
#include "EmptySpaceBlock.h"
#include "SolidBlock.h"
#include "BreakableBlock.h"
#include "BombBlock.h"

const char GameLevel::EMPTY_SPACE_CHAR				= 'E';
const char GameLevel::SOLID_BLOCK_CHAR				= 'S';
const char GameLevel::GREEN_BREAKABLE_CHAR		= 'G';
const char GameLevel::YELLOW_BREAKABLE_CHAR		= 'Y';
const char GameLevel::ORANGE_BREAKABLE_CHAR		= 'O';
const char GameLevel::RED_BREAKABLE_CHAR			= 'R';
const char GameLevel::BOMB_CHAR								= 'B';

// Private constructor, requires all the pieces that make up the level
GameLevel::GameLevel(unsigned int numBlocks, std::vector<std::vector<LevelPiece*>> pieces): currentLevelPieces(pieces),
piecesLeft(numBlocks) {
	assert(pieces.size() > 0);
	this->width = pieces[0].size();
	this->height = pieces.size();
}

// Destructor, clean up heap stuffs
GameLevel::~GameLevel() {

	// TODO: FIX THIS STUFF??? IT CRASHES...
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

			// Validate the block type and create teh appropriate object for that block type...
			LevelPiece* newPiece = NULL;
			switch (currBlock) {
				case EMPTY_SPACE_CHAR:
					newPiece = new EmptySpaceBlock(static_cast<unsigned int>(w), static_cast<unsigned int>(height - 1 - h));
					break;
				case SOLID_BLOCK_CHAR:
					newPiece = new SolidBlock(static_cast<unsigned int>(w), static_cast<unsigned int>(height - 1 - h));
					break;
				case GREEN_BREAKABLE_CHAR:
				case YELLOW_BREAKABLE_CHAR:
				case ORANGE_BREAKABLE_CHAR:
				case RED_BREAKABLE_CHAR:
					newPiece = new BreakableBlock(currBlock, static_cast<unsigned int>(w), static_cast<unsigned int>(height - 1 - h));
					break;
				case BOMB_CHAR:
					newPiece = new BombBlock(static_cast<unsigned int>(w), static_cast<unsigned int>(height - 1 - h));
					break;
				default:
					inFile.close();
					debug_output("ERROR: Invalid level interior value: " << currBlock << " at width = " << w << ", height = " << h); 
					return NULL;
			}
			assert(newPiece != NULL);
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
	
	// Make sure the provided indices are in the correct range
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
 * Call this when a level piece changes in this level. This function is meant to 
 * change the piece and manage the other level pieces accordingly.
 */
void GameLevel::PieceChanged(LevelPiece* pieceBefore, LevelPiece* pieceAfter) {
	assert(pieceBefore != NULL);
	assert(pieceAfter != NULL);

	// EVENT: Level piece has changed...
	GameEventManager::Instance()->ActionLevelPieceChanged(*pieceBefore, *pieceAfter);

	if (pieceBefore != pieceAfter) {
		// Find out if a vital piece was destroyed (i.e., the player is closer to finishing the level)
		bool isVitalBeforeCollision = pieceBefore->MustBeDestoryedToEndLevel();
		bool isVitalAfterCollision	= pieceAfter->MustBeDestoryedToEndLevel();

		// Update the number of pieces that need to be destroyed to end the level
		if (isVitalBeforeCollision && !isVitalAfterCollision) {
			// In this case a block that was vital has been destroyed
			this->piecesLeft--;
			assert(this->piecesLeft >= 0);
		}

		// Replace the old piece with the new one...
		assert(pieceBefore->GetHeightIndex() == pieceAfter->GetHeightIndex());
		assert(pieceBefore->GetWidthIndex()  == pieceAfter->GetWidthIndex());
		unsigned int hIndex = pieceAfter->GetHeightIndex();
		unsigned int wIndex = pieceAfter->GetWidthIndex();
		this->currentLevelPieces[hIndex][wIndex] = pieceAfter;

		// Update the neighbour's bounds...
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex-1); // left
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex); // bottom
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex+1); // right
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex); // top
	}
	else {
		// Inline: in this case there was a change within the piece object itself
		// since both the before and after objects are the same.
	}
}

/**
 * Private helper function for finding a set of levelpieces within the given range of values
 * indexing along the x and y axis.
 * Return: Set of levelpieces included in the given bounds.
 */
std::set<LevelPiece*> GameLevel::IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax) const {
	std::set<LevelPiece*> colliders;

	// Do some correction of x-axis index values if the ball goes out of bounds...
	if (xIndexMin < 0 || xIndexMin >= static_cast<float>(this->width)) {
		if (xIndexMax >= static_cast<float>(this->width) || xIndexMax < 0) {
			return colliders;
		}
		xIndexMin = xIndexMax;
	}
	else if (xIndexMax >= static_cast<float>(this->width) || xIndexMax < 0) {
		xIndexMax = xIndexMin;
	}

	// Do some correction of y-axis index values if the ball goes out of bounds...
	if (yIndexMin < 0 || yIndexMin >= static_cast<float>(this->height)) {
		if (yIndexMax >= static_cast<float>(this->height) || yIndexMax < 0) {
			return colliders;
		}
		yIndexMin = yIndexMax;
	}
	else if (yIndexMax >= static_cast<float>(this->height) || yIndexMax < 0) {
		yIndexMax = yIndexMin;
	}
	
	assert(xIndexMin <= xIndexMax);
	assert(yIndexMin <= yIndexMax);
	
	if (xIndexMax == xIndexMin) {
		if (yIndexMax == yIndexMin) {
			// Only one collision point
			colliders.insert(this->currentLevelPieces[yIndexMin][xIndexMin]);
		}
		else {
			// Some number of collisions along the y-axis
			for (int y = yIndexMin; y <= yIndexMax; y++) {
				colliders.insert(this->currentLevelPieces[y][xIndexMin]);
			}
		}
	}
	else {
			// No matter what there are some number of collisions along the x-axis
		for (int x = xIndexMin; x <= xIndexMax; x++) {
			colliders.insert(this->currentLevelPieces[yIndexMin][x]);
		}

		if (yIndexMax != yIndexMin) {
			// Add the left overs...
			for (int x = xIndexMin; x <= xIndexMax; x++) {
				colliders.insert(this->currentLevelPieces[yIndexMax][x]);
			}
		}
	}

	return colliders;
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given gameball.
 * Returns: array of unique LevelPieces that are possibly colliding with b.
 */
std::set<LevelPiece*> GameLevel::GetCollisionCandidates(const GameBall& b) const {

	// Get the ball boundry and use it to figure out what levelpieces are relevant
	Collision::Circle2D ballBounds = b.GetBounds();
	Point2D ballCenter = ballBounds.Center();

	// Find the non-rounded max and min indices to look at along the x and y axis
	float xNonAdjustedIndex = ballCenter[0] / LevelPiece::PIECE_WIDTH;
	int xIndexMax = static_cast<int>(floorf(xNonAdjustedIndex + ballBounds.Radius())); 
	int xIndexMin = static_cast<int>(floorf(xNonAdjustedIndex - ballBounds.Radius()));
	
	float yNonAdjustedIndex = ballCenter[1] / LevelPiece::PIECE_HEIGHT;
	int yIndexMax = static_cast<int>(floorf(yNonAdjustedIndex + ballBounds.Radius()));
	int yIndexMin = static_cast<int>(floorf(yNonAdjustedIndex - ballBounds.Radius()));

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given projectile.
 * Returns: array of unique LevelPieces that are possibly colliding with p.
 */
std::set<LevelPiece*> GameLevel::GetCollisionCandidates(const Projectile& p) const {
	Point2D projectileCenter = p.GetPosition();

	// Find the non-rounded max and min indices to look at along the x and y axis
	float xNonAdjustedIndex = projectileCenter[0] / LevelPiece::PIECE_WIDTH;
	int xIndexMax = static_cast<int>(floorf(xNonAdjustedIndex + p.GetHalfWidth())); 
	int xIndexMin = static_cast<int>(floorf(xNonAdjustedIndex - p.GetHalfWidth()));
	
	float yNonAdjustedIndex = projectileCenter[1] / LevelPiece::PIECE_HEIGHT;
	int yIndexMax = static_cast<int>(floorf(yNonAdjustedIndex + p.GetHalfHeight()));
	int yIndexMin = static_cast<int>(floorf(yNonAdjustedIndex - p.GetHalfHeight()));

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}