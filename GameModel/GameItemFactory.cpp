/**
 * GameItemFactory.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
#include "PoisonPaddleItem.h"
#include "StickyPaddleItem.h"
#include "PaddleCamItem.h"
#include "BallCamItem.h"
#include "LaserBeamPaddleItem.h"
#include "GravityBallItem.h"
#include "RocketPaddleItem.h"
#include "CrazyBallItem.h"
#include "ShieldPaddleItem.h"

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel) {
	assert(gameModel != NULL);
	GameItem::ItemType randomItemType = GameItemFactory::CreateRandomItemType(gameModel);
	return GameItemFactory::CreateItem(randomItemType, spawnOrigin, gameModel);
}

/**
 * Creates a random item type.
 */
GameItem::ItemType GameItemFactory::CreateRandomItemType(GameModel *gameModel) {
	assert(gameModel != NULL);
	unsigned int randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % TOTAL_NUM_OF_ITEMS;
	
	// Avoid insane numbers of balls, if we're going to spawn more balls and there's already a lot
	// of them try to pick another item...
	if (gameModel->GetGameBalls().size() > 3 && (randomValue == GameItem::MultiBall5Item || randomValue == GameItem::MultiBall3Item)) {
		randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % TOTAL_NUM_OF_ITEMS;
	}
	return static_cast<GameItem::ItemType>(randomValue);
}

/**
 * Creates a item based on the given item type; hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, GameModel *gameModel) {
	
	switch (type) {

		case GameItem::BallSpeedUpItem:
			return new BallSpeedItem(BallSpeedItem::FastBall, spawnOrigin, gameModel);
		
		case GameItem::BallSlowDownItem:
			return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);

		case GameItem::UberBallItem:
			return new UberBallItem(spawnOrigin, gameModel);

		case GameItem::InvisiBallItem:
			return new InvisiBallItem(spawnOrigin, gameModel);

		case GameItem::GhostBallItem:
			return new GhostBallItem(spawnOrigin, gameModel);

		case GameItem::LaserBulletPaddleItem:
			return new LaserPaddleItem(spawnOrigin, gameModel);

		case GameItem::MultiBall3Item:
			return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::ThreeMultiBalls);
	
		case GameItem::MultiBall5Item:
			return new MultiBallItem(spawnOrigin, gameModel, MultiBallItem::FiveMultiBalls);

		case GameItem::PaddleGrowItem:
			return new PaddleSizeItem(PaddleSizeItem::GrowPaddle, spawnOrigin, gameModel);

		case GameItem::PaddleShrinkItem:
			return new PaddleSizeItem(PaddleSizeItem::ShrinkPaddle, spawnOrigin, gameModel);

		case GameItem::BallShrinkItem:
			return new BallSizeItem(BallSizeItem::ShrinkBall, spawnOrigin, gameModel);

		case GameItem::BallGrowItem:
			return new BallSizeItem(BallSizeItem::GrowBall, spawnOrigin, gameModel);

		case GameItem::BlackoutItem:
			return new BlackoutItem(spawnOrigin, gameModel);
		
		case GameItem::UpsideDownItem:
			return new UpsideDownItem(spawnOrigin, gameModel);

		case GameItem::BallSafetyNetItem:
			return new BallSafetyNetItem(spawnOrigin, gameModel);

		case GameItem::OneUpItem:
			return new OneUpItem(spawnOrigin, gameModel);

		case GameItem::PoisonPaddleItem:
			return new PoisonPaddleItem(spawnOrigin, gameModel);
		
		case GameItem::StickyPaddleItem:
			return new StickyPaddleItem(spawnOrigin, gameModel);

		case GameItem::PaddleCamItem:
			return new PaddleCamItem(spawnOrigin, gameModel);
		
		case GameItem::BallCamItem:
			return new BallCamItem(spawnOrigin, gameModel);

		case GameItem::LaserBeamPaddleItem:
			return new LaserBeamPaddleItem(spawnOrigin, gameModel);

		case GameItem::GravityBallItem:
			return new GravityBallItem(spawnOrigin, gameModel);

		case GameItem::RocketPaddleItem:
			return new RocketPaddleItem(spawnOrigin, gameModel);

		case GameItem::CrazyBallItem:
			return new CrazyBallItem(spawnOrigin, gameModel);

		case GameItem::ShieldPaddleItem:
			return new ShieldPaddleItem(spawnOrigin, gameModel);

		default:
			assert(false);
			break;
	}

	return NULL;
}