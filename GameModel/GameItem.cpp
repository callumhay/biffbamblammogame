#include "GameItem.h"
#include "GameModel.h"

// Width and Height constants for items
const float GameItem::ITEM_WIDTH	= 2.45f;
const float GameItem::ITEM_HEIGHT	= 1.0f;
const float GameItem::HALF_ITEM_WIDTH		= ITEM_WIDTH / 2.0f;
const float GameItem::HALF_ITEM_HEIGHT	= ITEM_HEIGHT / 2.0f;

const float GameItem::SPEED_OF_DESCENT	= 5.0f;

GameItem::GameItem(const std::string& name, const Point2D &spawnOrigin, GameModel *gameModel, const ItemType type) : 
	name(name), center(spawnOrigin), gameModel(gameModel), type(type), isActive(false) {
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

	// Create basic AABB for both paddle and item and then
	// check for a collision in 2D.	
	Collision::AABB2D paddleAABB(paddle.GetCenterPosition() - Vector2D(paddle.GetHalfWidthTotal(), paddle.GetHalfHeight()), 
		                           paddle.GetCenterPosition() + Vector2D(paddle.GetHalfWidthTotal(), paddle.GetHalfHeight()));
	Collision::AABB2D itemAABB(this->GetCenter() - Vector2D(HALF_ITEM_WIDTH, HALF_ITEM_HEIGHT), 
														 this->GetCenter() + Vector2D(HALF_ITEM_WIDTH, HALF_ITEM_HEIGHT));
	return Collision::IsCollision(paddleAABB, itemAABB);
}