#include "GameItemFactory.h"
#include "GameModel.h"

// GameItem-related classes
#include "GameItem.h"
#include "SlowBallItem.h"

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	// TODO: more items go here...

	return new SlowBallItem(spawnOrigin, gameModel);
}