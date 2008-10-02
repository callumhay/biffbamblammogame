#include "GameItem.h"
#include "GameModel.h"

#include "../Utils/Vector.h"

// Width and Height constants for items
const float GameItem::ITEM_WIDTH	= 2.5f;
const float GameItem::ITEM_HEIGHT	= 1.0f;
const float GameItem::HALF_ITEM_WIDTH		= ITEM_WIDTH / 2.0f;
const float GameItem::HALF_ITEM_HEIGHT	= ITEM_HEIGHT / 2.0f;

const float GameItem::SPEED_OF_DESCENT	= 15.0f;

GameItem::GameItem(GameModel *gameModel) : gameModel(gameModel) {
	assert(gameModel != NULL);
}

GameItem::~GameItem() {
}

/**
 * This function will execute the movement / action of the item as it progresses
 * through game time, given in seconds.
 */
void GameItem::Tick(double seconds) {
	// With every tick we simply move the item down at its
	// descent speed, all other activity (e.g., player paddle collision) 
	// is taken care of by the game

	Vector2D itemVel = Vector2D(0, - SPEED_OF_DESCENT * seconds);
	this->center = this->center + itemVel;
}

/**
 * This function will check for collisions with this item and
 * the given player paddle.
 * Returns: true on collision, false otherwise.
 */
bool GameItem::CollisionCheck(const PlayerPaddle &paddle) {
	return false;
}