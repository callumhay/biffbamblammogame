#include "GameItemFactory.h"
#include "GameModel.h"

// GameItem-related classes
#include "GameItem.h"
#include "BallSpeedItem.h"
#include "UberBallItem.h"
#include "InvisiBallItem.h"
#include "GhostBallItem.h"
#include "LaserPaddleItem.h"
#include "MultiBallItem.h"
#include "PaddleSizeItem.h"
#include "BallSizeItem.h"
#include "BlackoutItem.h"
#include "UpsideDownItem.h"
#include "BallSafetyNetItem.h"
#include "OneUpItem.h"

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	assert(gameModel != NULL);
	
	unsigned int randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % TOTAL_NUM_OF_ITEMS;
	
	// Avoid insane numbers of balls, if we're going to spawn more balls and there's already a lot
	// of them try to pick another item...
	if (gameModel->GetGameBalls().size() > 3 && randomValue == 7) {
		randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % TOTAL_NUM_OF_ITEMS;
	}
	
	// TODO: more items go here...
	switch (randomValue) {	// switch is faster than a for loop... trade-off dynamic for speed
		case 0:
			return new BallSpeedItem(BallSpeedItem::FastBall, spawnOrigin, gameModel);					// bad
		case 1:
			return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);					// good
		case 2:
			return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::ThreeMultiBalls);		// good
		case 3:
			return new InvisiBallItem(spawnOrigin, gameModel);																	// bad
		case 4:
			return new GhostBallItem(spawnOrigin, gameModel);																		// neutral
		case 5:
			return new LaserPaddleItem(spawnOrigin, gameModel);																	// good
		case 6:
			return new UberBallItem(spawnOrigin, gameModel);																		// good
		case 7:
			return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::FiveMultiBalls);		// good
		case 8:
			return new PaddleSizeItem(PaddleSizeItem::ShrinkPaddle, spawnOrigin, gameModel);		// bad
		case 9:
			return new PaddleSizeItem(PaddleSizeItem::GrowPaddle, spawnOrigin, gameModel);			// good
		case 10:
			return new BallSizeItem(BallSizeItem::ShrinkBall, spawnOrigin, gameModel);					// bad
		case 11:
			return new BallSizeItem(BallSizeItem::GrowBall, spawnOrigin, gameModel);						// good
		case 12:
			return new BlackoutItem(spawnOrigin, gameModel);																		// bad
		case 13:
			return new UpsideDownItem(spawnOrigin, gameModel);																	// bad
		case 14:
			return new BallSafetyNetItem(spawnOrigin, gameModel);																// good
		case 15:
			return new OneUpItem(spawnOrigin, gameModel);																				// good
		default:
			assert(false);
	}

	return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);
}

#ifdef _DEBUG
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
	else if (itemName == LaserPaddleItem::LASER_PADDLE_ITEM_NAME) {
		return new LaserPaddleItem(spawnOrigin, gameModel);
	}
	else if (itemName == MultiBallItem::MULTI3_BALL_ITEM_NAME) {
		return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::ThreeMultiBalls);
	}
	else if (itemName == MultiBallItem::MULTI5_BALL_ITEM_NAME) {
		return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::FiveMultiBalls);
	}
	else if (itemName == PaddleSizeItem::PADDLE_GROW_ITEM_NAME) {
		return new PaddleSizeItem(PaddleSizeItem::GrowPaddle, spawnOrigin, gameModel);
	}
	else if (itemName == PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME) {
		return new PaddleSizeItem(PaddleSizeItem::ShrinkPaddle, spawnOrigin, gameModel);
	}
	else if (itemName == BallSizeItem::BALL_SHRINK_ITEM_NAME) {
		return new BallSizeItem(BallSizeItem::ShrinkBall, spawnOrigin, gameModel);
	}
	else if (itemName == BallSizeItem::BALL_GROW_ITEM_NAME) {
		return new BallSizeItem(BallSizeItem::GrowBall, spawnOrigin, gameModel);
	}
	else if (itemName == BlackoutItem::BLACKOUT_ITEM_NAME) {
		return new BlackoutItem(spawnOrigin, gameModel);
	}
	else if (itemName == UpsideDownItem::UPSIDEDOWN_ITEM_NAME) {
		return new UpsideDownItem(spawnOrigin, gameModel);
	}
	else if (itemName == BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME) {
		return new BallSafetyNetItem(spawnOrigin, gameModel);
	}
	else if (itemName == OneUpItem::ONE_UP_ITEM_NAME) {
		return new OneUpItem(spawnOrigin, gameModel);
	}
	assert(false);
	return NULL;
}
#endif