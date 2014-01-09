/**
 * GameItemFactory.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
#include "LifeUpItem.h"
#include "PoisonPaddleItem.h"
#include "StickyPaddleItem.h"
#include "PaddleCamItem.h"
#include "BallCamItem.h"
#include "LaserBeamPaddleItem.h"
#include "GravityBallItem.h"
#include "RocketPaddleItem.h"
#include "CrazyBallItem.h"
#include "ShieldPaddleItem.h"
#include "FireBallItem.h"
#include "IceBallItem.h"
#include "OmniLaserBallItem.h"
#include "InvisiPaddleItem.h"
#include "MagnetPaddleItem.h"
#include "PaddleMineLauncherItem.h"
#include "RemoteControlRocketItem.h"
#include "FlameBlasterPaddleItem.h"
#include "RandomItem.h"

GameItemFactory* GameItemFactory::instance = NULL;

GameItemFactory::GameItemFactory() {
	// Initialize the mapping of game item names to types
	itemNameToTypeMap.insert(std::make_pair(BallSpeedItem::FAST_BALL_ITEM_NAME,                         GameItem::BallSpeedUpItem));
	itemNameToTypeMap.insert(std::make_pair(BallSpeedItem::SLOW_BALL_ITEM_NAME,                         GameItem::BallSlowDownItem));
	itemNameToTypeMap.insert(std::make_pair(UberBallItem::UBER_BALL_ITEM_NAME,                          GameItem::UberBallItem));
	itemNameToTypeMap.insert(std::make_pair(InvisiBallItem::INVISI_BALL_ITEM_NAME,                      GameItem::InvisiBallItem));
	itemNameToTypeMap.insert(std::make_pair(GhostBallItem::GHOST_BALL_ITEM_NAME,                        GameItem::GhostBallItem));
	itemNameToTypeMap.insert(std::make_pair(LaserPaddleItem::LASER_PADDLE_ITEM_NAME,                    GameItem::LaserBulletPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(MultiBallItem::MULTI3_BALL_ITEM_NAME,                       GameItem::MultiBall3Item));
	itemNameToTypeMap.insert(std::make_pair(MultiBallItem::MULTI5_BALL_ITEM_NAME,                       GameItem::MultiBall5Item));
	itemNameToTypeMap.insert(std::make_pair(PaddleSizeItem::PADDLE_GROW_ITEM_NAME,                      GameItem::PaddleGrowItem));
	itemNameToTypeMap.insert(std::make_pair(PaddleSizeItem::PADDLE_SHRINK_ITEM_NAME,                    GameItem::PaddleShrinkItem));
	itemNameToTypeMap.insert(std::make_pair(BallSizeItem::BALL_SHRINK_ITEM_NAME,                        GameItem::BallShrinkItem));
	itemNameToTypeMap.insert(std::make_pair(BallSizeItem::BALL_GROW_ITEM_NAME,                          GameItem::BallGrowItem));
	itemNameToTypeMap.insert(std::make_pair(BlackoutItem::BLACKOUT_ITEM_NAME,                           GameItem::BlackoutItem));
	itemNameToTypeMap.insert(std::make_pair(UpsideDownItem::UPSIDEDOWN_ITEM_NAME,                       GameItem::UpsideDownItem));
	itemNameToTypeMap.insert(std::make_pair(BallSafetyNetItem::BALL_SAFETY_NET_ITEM_NAME,               GameItem::BallSafetyNetItem));
	itemNameToTypeMap.insert(std::make_pair(PoisonPaddleItem::POISON_PADDLE_ITEM_NAME,                  GameItem::PoisonPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(StickyPaddleItem::STICKY_PADDLE_ITEM_NAME,                  GameItem::StickyPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(PaddleCamItem::PADDLE_CAM_ITEM_NAME,                        GameItem::PaddleCamItem));
	itemNameToTypeMap.insert(std::make_pair(BallCamItem::BALL_CAM_ITEM_NAME,                            GameItem::BallCamItem));
	itemNameToTypeMap.insert(std::make_pair(LaserBeamPaddleItem::LASER_BEAM_PADDLE_ITEM_NAME,           GameItem::LaserBeamPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(GravityBallItem::GRAVITY_BALL_ITEM_NAME,                    GameItem::GravityBallItem));
	itemNameToTypeMap.insert(std::make_pair(RocketPaddleItem::ROCKET_PADDLE_ITEM_NAME,                  GameItem::RocketPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(CrazyBallItem::CRAZY_BALL_ITEM_NAME,                        GameItem::CrazyBallItem));
	itemNameToTypeMap.insert(std::make_pair(ShieldPaddleItem::SHIELD_PADDLE_ITEM_NAME,                  GameItem::ShieldPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(FireBallItem::FIRE_BALL_ITEM_NAME,                          GameItem::FireBallItem));
	itemNameToTypeMap.insert(std::make_pair(IceBallItem::ICE_BALL_ITEM_NAME,                            GameItem::IceBallItem));
    itemNameToTypeMap.insert(std::make_pair(OmniLaserBallItem::OMNI_LASER_BALL_ITEM_NAME,               GameItem::OmniLaserBallItem));
    itemNameToTypeMap.insert(std::make_pair(InvisiPaddleItem::INVISI_PADDLE_ITEM_NAME,                  GameItem::InvisiPaddleItem));
    itemNameToTypeMap.insert(std::make_pair(MagnetPaddleItem::MAGNET_PADDLE_ITEM_NAME,                  GameItem::MagnetPaddleItem));
    itemNameToTypeMap.insert(std::make_pair(PaddleMineLauncherItem::MINE_LAUNCHER_PADDLE_ITEM_NAME,     GameItem::MineLauncherPaddleItem));
    itemNameToTypeMap.insert(std::make_pair(RemoteControlRocketItem::REMOTE_CONTROL_ROCKET_ITEM_NAME,   GameItem::RemoteCtrlRocketItem));
    itemNameToTypeMap.insert(std::make_pair(FlameBlasterPaddleItem::FLAMETHROWER_PADDLE_ITEM_NAME,      GameItem::FlameBlasterPaddleItem));
    //itemNameToTypeMap.insert(std::make_pair(IceBeamPaddleItem::ICEBEAM_PADDLE_ITEM_NAME,                GameItem::IceBeamPaddleItem));
	itemNameToTypeMap.insert(std::make_pair(RandomItem::RANDOM_ITEM_NAME,                               GameItem::RandomItem));
	
	// Establish the set of ALL items except the random item!
	allItemTypes.insert(GameItem::BallSpeedUpItem);
	allItemTypes.insert(GameItem::BallSlowDownItem);
	allItemTypes.insert(GameItem::UberBallItem);
	allItemTypes.insert(GameItem::InvisiBallItem);
	allItemTypes.insert(GameItem::GhostBallItem);
	allItemTypes.insert(GameItem::LaserBulletPaddleItem);
	allItemTypes.insert(GameItem::MultiBall3Item);
	allItemTypes.insert(GameItem::MultiBall5Item);
	allItemTypes.insert(GameItem::PaddleGrowItem);
	allItemTypes.insert(GameItem::PaddleShrinkItem);
	allItemTypes.insert(GameItem::BallGrowItem);
	allItemTypes.insert(GameItem::BallShrinkItem);
	allItemTypes.insert(GameItem::BlackoutItem);
	allItemTypes.insert(GameItem::UpsideDownItem);
	allItemTypes.insert(GameItem::BallSafetyNetItem);
	allItemTypes.insert(GameItem::PoisonPaddleItem);
	allItemTypes.insert(GameItem::StickyPaddleItem);
	allItemTypes.insert(GameItem::PaddleCamItem);
	allItemTypes.insert(GameItem::BallCamItem);
	allItemTypes.insert(GameItem::LaserBeamPaddleItem);
	allItemTypes.insert(GameItem::GravityBallItem);
	allItemTypes.insert(GameItem::RocketPaddleItem);
	allItemTypes.insert(GameItem::CrazyBallItem);
	allItemTypes.insert(GameItem::ShieldPaddleItem);
	allItemTypes.insert(GameItem::FireBallItem);
	allItemTypes.insert(GameItem::IceBallItem);
    allItemTypes.insert(GameItem::OmniLaserBallItem);
    allItemTypes.insert(GameItem::InvisiPaddleItem);
    allItemTypes.insert(GameItem::MagnetPaddleItem);
    allItemTypes.insert(GameItem::MineLauncherPaddleItem);
    allItemTypes.insert(GameItem::RemoteCtrlRocketItem);
    allItemTypes.insert(GameItem::FlameBlasterPaddleItem);
    //allItemTypes.insert(GameItem::IceBeamPaddleItem);
	
	// Establish the Power-up item set
	allPowerUpItemTypes.insert(GameItem::BallSlowDownItem);
	allPowerUpItemTypes.insert(GameItem::UberBallItem);
	allPowerUpItemTypes.insert(GameItem::LaserBulletPaddleItem);
	allPowerUpItemTypes.insert(GameItem::MultiBall3Item);
	allPowerUpItemTypes.insert(GameItem::MultiBall5Item);
	allPowerUpItemTypes.insert(GameItem::PaddleGrowItem);
	allPowerUpItemTypes.insert(GameItem::BallGrowItem);
	allPowerUpItemTypes.insert(GameItem::BallSafetyNetItem);
	allPowerUpItemTypes.insert(GameItem::StickyPaddleItem);
	allPowerUpItemTypes.insert(GameItem::LaserBeamPaddleItem);
	allPowerUpItemTypes.insert(GameItem::RocketPaddleItem);
    allPowerUpItemTypes.insert(GameItem::MineLauncherPaddleItem);
    allPowerUpItemTypes.insert(GameItem::RemoteCtrlRocketItem);

	// Establish the Power-neutral item set
	allPowerNeutralItemTypes.insert(GameItem::GhostBallItem);
	allPowerNeutralItemTypes.insert(GameItem::GravityBallItem);
	allPowerNeutralItemTypes.insert(GameItem::CrazyBallItem);
	allPowerNeutralItemTypes.insert(GameItem::ShieldPaddleItem);
	allPowerNeutralItemTypes.insert(GameItem::FireBallItem);
    allPowerNeutralItemTypes.insert(GameItem::FlameBlasterPaddleItem);
	allPowerNeutralItemTypes.insert(GameItem::IceBallItem);
    //allPowerNeutralItemTypes.insert(GameItem::IceBeamPaddleItem);
    allPowerNeutralItemTypes.insert(GameItem::OmniLaserBallItem);
    allPowerNeutralItemTypes.insert(GameItem::MagnetPaddleItem);
    allPowerNeutralItemTypes.insert(GameItem::RandomItem);

	// Establish the Power-down item set
	allPowerDownItemTypes.insert(GameItem::BallShrinkItem);
	allPowerDownItemTypes.insert(GameItem::BallSpeedUpItem);
	allPowerDownItemTypes.insert(GameItem::BlackoutItem);
	allPowerDownItemTypes.insert(GameItem::InvisiBallItem);
	allPowerDownItemTypes.insert(GameItem::PaddleCamItem);
	allPowerDownItemTypes.insert(GameItem::BallCamItem);
	allPowerDownItemTypes.insert(GameItem::PaddleShrinkItem);
	allPowerDownItemTypes.insert(GameItem::PoisonPaddleItem);
	allPowerDownItemTypes.insert(GameItem::UpsideDownItem);
    allPowerDownItemTypes.insert(GameItem::InvisiPaddleItem);

    atexit(GameItemFactory::DeleteInstance);
}

GameItemFactory::~GameItemFactory() {
}

/**
 * Creates a random item, could be either a power-up or down and
 * hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateRandomItem(const Point2D &spawnOrigin, GameModel *gameModel, bool allowRandomItemType) const {
	assert(gameModel != NULL);
	GameItem::ItemType randomItemType = GameItemFactory::CreateRandomItemType(gameModel, allowRandomItemType);
	return GameItemFactory::CreateItem(randomItemType, spawnOrigin, gameModel);
}

/**
 * Creates a random item type.
 */
GameItem::ItemType GameItemFactory::CreateRandomItemType(GameModel *gameModel, bool allowRandomItemType) const {
	assert(gameModel != NULL);
	unsigned int numItems = this->allItemTypes.size();
	if (allowRandomItemType) {
		numItems++;
	}
	unsigned int randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % numItems;

	// Avoid insane numbers of balls, if we're going to spawn more balls and there's already a lot
	// of them try to pick another item...
	if (gameModel->GetGameBalls().size() > 3 && (randomValue == GameItem::MultiBall5Item || randomValue == GameItem::MultiBall3Item)) {
		randomValue = Randomizer::GetInstance()->RandomUnsignedInt() % numItems;
	}
	assert((!allowRandomItemType && randomValue != GameItem::RandomItem) || allowRandomItemType);

	return static_cast<GameItem::ItemType>(randomValue);
}

/**
 * Builds a game item type from the given item name.
 */
GameItem::ItemType GameItemFactory::GetItemTypeFromName(const std::string& itemName) const {
	std::map<std::string, GameItem::ItemType>::const_iterator findIter = this->itemNameToTypeMap.find(itemName.c_str());
	assert(findIter != this->itemNameToTypeMap.end());
	return findIter->second;
}

/**
 * Creates a item based on the given item type; hands the new object to the caller.
 * NOTE: The caller will be responsible for its destruction!
 */
GameItem* GameItemFactory::CreateItem(GameItem::ItemType type, const Point2D &spawnOrigin, GameModel *gameModel) const {
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

		case GameItem::LifeUpItem:
			return new LifeUpItem(spawnOrigin, gameModel);

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

		case GameItem::FireBallItem:
			return new FireBallItem(spawnOrigin, gameModel);

		case GameItem::IceBallItem:
			return new IceBallItem(spawnOrigin, gameModel);

        case GameItem::OmniLaserBallItem:
            return new OmniLaserBallItem(spawnOrigin, gameModel);

        case GameItem::InvisiPaddleItem:
            return new InvisiPaddleItem(spawnOrigin, gameModel);

        case GameItem::MagnetPaddleItem:
            return new MagnetPaddleItem(spawnOrigin, gameModel);

        case GameItem::MineLauncherPaddleItem:
            return new PaddleMineLauncherItem(spawnOrigin, gameModel);
        
        case GameItem::RemoteCtrlRocketItem:
            return new RemoteControlRocketItem(spawnOrigin, gameModel);

        case GameItem::FlameBlasterPaddleItem:
            return new FlameBlasterPaddleItem(spawnOrigin, gameModel);

        //case GameItem::IceBeamPaddleItem:
            //return new IceBeamPaddleItem(spawnOrigin, gameModel);

		// Random item is a very special kind of item that can't be generated anywhere else but here
		case GameItem::RandomItem:
			return new RandomItem(spawnOrigin, gameModel);

		default:
			assert(false);
			break;
	}

	return NULL;
}

GameItem* GameItemFactory::CreateRandomItemForCurrentLevel(const Point2D &spawnOrigin, GameModel *gameModel, bool allowRandomItemType) const {
	assert(gameModel != NULL);
    
    // Check the special case where no item drops are allowed for the current level...
    const GameLevel* currGameLevel = gameModel->GetCurrentLevel();
	assert(currGameLevel != NULL);
	const std::vector<GameItem::ItemType>& allowableItemDrops = currGameLevel->GetAllowableItemDropTypes();
    if (allowableItemDrops.empty()) {
        return new BallSpeedItem(BallSpeedItem::SlowBall, spawnOrigin, gameModel);;
    }

	GameItem::ItemType randomItemType = GameItemFactory::CreateRandomItemTypeForCurrentLevel(gameModel, allowRandomItemType);
	return GameItemFactory::CreateItem(randomItemType, spawnOrigin, gameModel);
}

/**
 * Generate a item type that is within the types allowed for the current level of the game.
 * If allowRandomItemType is set to true then a random item type may be generated as well.
 */
GameItem::ItemType GameItemFactory::CreateRandomItemTypeForCurrentLevel(GameModel *gameModel, bool allowRandomItemType) const {
    assert(gameModel != NULL);

    static GameItem::ItemType lastGeneratedItemType = GameItem::MultiBall5Item;

	// Grab the current game level and get the allowable item drops for it
	const GameLevel* currGameLevel = gameModel->GetCurrentLevel();
	assert(currGameLevel != NULL);
	const std::vector<GameItem::ItemType>& allowableItemDrops = currGameLevel->GetAllowableItemDropTypes();
	assert(!allowableItemDrops.empty());

	// If we've enabled generation of random items then we allow the possible generatation of one...
	size_t randomNum = 0;
	if (allowRandomItemType) {

		randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % (allowableItemDrops.size() + currGameLevel->GetRandomItemDropProbabilityNum());
		if (randomNum >= allowableItemDrops.size()) {

            // Check for consecutive item drops...
            if (lastGeneratedItemType == GameItem::RandomItem) {
                if (Randomizer::GetInstance()->RandomNumZeroToOne() > GameModelConstants::GetInstance()->PROB_OF_CONSECUTIVE_SAME_ITEM_DROP) {
                    // Don't allow the consecutive drop...
                    randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size();
                }
                else {
                    lastGeneratedItemType = GameItem::RandomItem;
			        return lastGeneratedItemType;
                }
            }
            else {
                lastGeneratedItemType = GameItem::RandomItem;
			    return lastGeneratedItemType;
            }
		}
	}
	else {
		randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size();
	}

    // special stuff: if the game is on easy difficulty and the random item was a 'bad' item
    // and we have some probability then we look for another item,
    // vice-versa for hard difficulty
    switch (gameModel->GetDifficulty()) {
        case GameModel::EasyDifficulty:
            // 1/4 chance of re-rolling when it's a power-down
            if (this->allPowerDownItemTypes.find(allowableItemDrops.at(randomNum)) != this->allPowerDownItemTypes.end()) {
                if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
                    randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size();
                }
            }
            break;

        case GameModel::HardDifficulty:
            // 1/4 chance of re-rolling when it's a power-up
            if (this->allPowerUpItemTypes.find(allowableItemDrops.at(randomNum)) != this->allPowerUpItemTypes.end()) {
                if (Randomizer::GetInstance()->RandomUnsignedInt() % 4 == 0) {
                    randomNum = Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size();
                }
            }
            break;

        default:
            break;
    }

    GameItem::ItemType currRandomDropType = allowableItemDrops.at(randomNum);
    if (currRandomDropType == lastGeneratedItemType) {
        debug_output("Doing consecutive item drop test...");
        if (Randomizer::GetInstance()->RandomNumZeroToOne() > GameModelConstants::GetInstance()->PROB_OF_CONSECUTIVE_SAME_ITEM_DROP) {
            // Don't allow a consecutive item drop of the same type...
            randomNum = (randomNum + (Randomizer::GetInstance()->RandomUnsignedInt() % allowableItemDrops.size())) % allowableItemDrops.size();
            currRandomDropType = allowableItemDrops.at(randomNum);
        }
    }

    lastGeneratedItemType = currRandomDropType;
	return lastGeneratedItemType;
}

GameItem::ItemDisposition GameItemFactory::GetItemTypeDisposition(const GameItem::ItemType& itemType) const {
	if (this->allPowerUpItemTypes.find(itemType) != this->allPowerUpItemTypes.end()) {
		return GameItem::Good;
	}
    // Special case for life-up item
    if (itemType == GameItem::LifeUpItem) {
        return GameItem::Good;
    }

	else if (this->allPowerNeutralItemTypes.find(itemType) != this->allPowerNeutralItemTypes.end()) {
		return GameItem::Neutral;
	}
	
	return GameItem::Bad;
}
