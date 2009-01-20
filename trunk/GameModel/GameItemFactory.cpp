#include "GameItemFactory.h"
#include "GameModel.h"

// GameItem-related classes
#include "GameItem.h"
#include "BallSpeedItem.h"
#include "UberBallItem.h"
#include "InvisiBallItem.h"
#include "GhostBallItem.h"

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	assert(gameModel != NULL);
	
	int randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % TOTAL_NUM_OF_ITEMS;
	// TODO: more items go here...
	switch (randomValue) {	// switch is faster than a for loop... trade-off dynamic for speed
		case 0:
			return new BallSpeedItem(BallSpeedItem::FastBall, spawnOrigin, gameModel);
		case 1:
			return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
		case 2:
			return new UberBallItem(spawnOrigin, gameModel);
		case 3:
			return new InvisiBallItem(spawnOrigin, gameModel);
		case 4:
			return new GhostBallItem(spawnOrigin, gameModel);
		default:
			assert(false);
	}

	return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
}

#ifndef NDEBUG
GameItem* GameItemFactory::CreateItem(const std::string itemName, const Point2D &spawnOrigin, GameModel *gameModel) {
	if (itemName == BallSpeedItem::FAST_BALL_ITEM_NAME) {
		return new BallSpeedItem(BallSpeedItem::FastBall, spawnOrigin, gameModel);
	}
	else if (itemName == BallSpeedItem::SLOW_BALL_ITEM_NAME) {
		return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
	}
	else if (itemName == UberBallItem::UBER_BALL_ITEM_NAME) {
		return new UberBallItem(spawnOrigin, gameModel);
	}
	else if (itemName == InvisiBallItem::INVISI_BALL_ITEM_NAME) {
		return new InvisiBallItem(spawnOrigin, gameModel);
	}
	else if (itemName == GhostBallItem::GHOST_BALL_ITEM_NAME) {
		return new GhostBallItem(spawnOrigin, gameModel);
	}

	return NULL;
}
#endif