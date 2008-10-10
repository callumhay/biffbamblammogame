#include "GameItemFactory.h"
#include "GameModel.h"

// GameItem-related classes
#include "GameItem.h"
#include "SlowBallItem.h"
#include "FastBallItem.h"

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
			return new SlowBallItem(spawnOrigin, gameModel);
		case 1:
			return new FastBallItem(spawnOrigin, gameModel);
		default:
			assert(false);
	}

	return new SlowBallItem(spawnOrigin, gameModel);;
}