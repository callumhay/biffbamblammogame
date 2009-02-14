#include "LevelPiece.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameModel.h"
#include "GameItem.h"
#include "GameItemFactory.h"
#include "GameModelConstants.h"

#include "../BlammoEngine/BlammoEngine.h"

const float LevelPiece::PIECE_WIDTH = 2.5f;
const float LevelPiece::PIECE_HEIGHT = 1.0f;
const float LevelPiece::HALF_PIECE_WIDTH = PIECE_WIDTH / 2.0f;
const float LevelPiece::HALF_PIECE_HEIGHT = PIECE_HEIGHT / 2.0f;


LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc):
center(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT), wIndex(wLoc), hIndex(hLoc),
colour(1,1,1) {
}


LevelPiece::~LevelPiece() {
}

/**
 * Function for adding a possible item drop for the given level piece.
 */
void LevelPiece::AddPossibleItemDrop(GameModel *gameModel) {
	assert(gameModel != NULL);

	std::list<GameItem*>& currentItemsDropping = gameModel->GetLiveItems();
	// Make sure we don't drop more items than the max allowable...
	if (currentItemsDropping.size() >= GameModelConstants::GetInstance()->MAX_LIVE_ITEMS) {
		return;
	}

	// We will drop an item based on probablility
	// TODO: Add probability based off multiplier and score stuff...
	double itemDropProb = GameModelConstants::GetInstance()->PROB_OF_ITEM_DROP;
	double randomNum    = Randomizer::GetInstance()->RandomNumZeroToOne();
	
	//debug_output("Probability of drop: " << itemDropProb << " Number for deciding: " << randomNum);

	if (randomNum <= itemDropProb) {
		// Drop an item - create a random item and add it to the list...
		GameItem* newGameItem = GameItemFactory::CreateRandomItem(this->GetCenter(), gameModel);
		currentItemsDropping.push_back(newGameItem);
		// EVENT: Item has been created and added to the game
		GameEventManager::Instance()->ActionItemSpawned(*newGameItem);
	}
}

/**
 * Check for a collision of a given circle with this block.
 * Returns: true on collision as well as the normal of the line being collided with
 * and the distance from that line of the given circle; false otherwise.
 */
bool LevelPiece::CollisionCheck(const Circle2D& c, Vector2D& n, float &d) {
	if (this->IsNoBoundsPieceType()) {
		return false;
	}

	return this->bounds.Collide(c, n, d);
}

// Draws the boundry lines and normals for this level piece.
void LevelPiece::DebugDraw() const {
	this->bounds.DebugDraw();
}