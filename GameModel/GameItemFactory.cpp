#include "GameItemFactory.h"
#include "GameModel.h"

// GameItem-related classes
#include "GameItem.h"
#include "BallSpeedItem.h"
#include "UberBallItem.h"
#include "InvisiBallItem.h"

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	assert(gameModel != NULL);
	
	int randomValue = Randomizer::Random() % TOTAL_NUM_OF_ITEMS;
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
		default:
			assert(false);
	}

	return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
}

#ifndef NDEBUG
GameItem* GameItemFactory::CreateFastBallItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	return new BallSpeedItem(BallSpeedItem::FastBall, spawnOrigin, gameModel);
}

GameItem* GameItemFactory::CreateSlowBallItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
}
GameItem* GameItemFactory::CreateUberBallItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	return new UberBallItem(spawnOrigin, gameModel);
}
GameItem* GameItemFactory::CreateInvisiBallItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	return new InvisiBallItem(spawnOrigin, gameModel);
}
#endif