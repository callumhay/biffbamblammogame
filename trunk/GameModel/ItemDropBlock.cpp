/**
 * ItemDropBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ItemDropBlock.h"
#include "PaddleLaserBeam.h"
#include "GameItemFactory.h"
#include "EmptySpaceBlock.h"

// Amount of damage the block will take before dropping an item while being hit by a beam
const float ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP = 150.0f;
// Amount of time (in milliseconds) that must be waited until another item drop is allowed (for things other than beam damage)
const unsigned long ItemDropBlock::DISABLE_DROP_TIME = 1000;

ItemDropBlock::ItemDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), allowedItemDropTypes(droppableItemTypes), hitPointsBeforeNextDrop(ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP),
timeOfLastDrop(0) {
	assert(!droppableItemTypes.empty());

	// Prepare for the next random item...
	this->ChangeToNextItemDropType(false);
}

ItemDropBlock::~ItemDropBlock() {
}

bool ItemDropBlock::ProjectilePassesThrough(const Projectile* projectile) const {
	switch (projectile->GetType()) {
		
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			// When frozen, projectiles can pass through
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				return true;
			}
			break;

		default:
			break;
	}

	return false;
}

/**
 * An item drop block may actually be destroyed (specifically it can only
 * be destroyed by a collateral block falling on top of it)... hopefully the level designer doesn't depend
 * on it existing if they setup a level where it can be destroyed..
 */
LevelPiece* ItemDropBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		// EVENT: Ice was shattered
		GameEventManager::Instance()->ActionBlockIceShattered(*this);
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
		assert(success);
	}

    // Only collateral blocks and tesla lighting can destroy this block, otherwise the block is just triggered
    if (method != LevelPiece::CollateralDestruction && method != LevelPiece::TeslaDestruction) {
        this->Triggered(gameModel);
        return this;
    }

    // Drop one last item before death...
    this->timeOfLastDrop = 0;
    this->AttemptToDropAnItem(gameModel);

    // EVENT: Block is being destroyed
    GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

    // Tell the level that this piece has changed to empty...
    GameLevel* level = gameModel->GetCurrentLevel();
    LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
    level->PieceChanged(gameModel, this, emptyPiece, method);

    // Obliterate all that is left of this block...
    LevelPiece* tempThis = this;
    delete tempThis;
    tempThis = NULL;

    return emptyPiece;
}

// When balls collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	if (ball.IsLastPieceCollidedWith(this)) {
		return this;
	}

	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		bool isInIceCube = this->HasStatus(LevelPiece::IceCubeStatus);
		if (!isFireBall && isInIceCube) {
			// EVENT: Ice was shattered
			GameEventManager::Instance()->ActionBlockIceShattered(*this);
		}

		// Unfreeze a frozen solid block
		if (isInIceCube) {
			bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
            UNUSED_VARIABLE(success);
			assert(success);
		}
		else {
			this->AttemptToDropAnItem(gameModel);
		}
	}

	// Tell the ball that it collided with this block
	ball.SetLastPieceCollidedWith(this);
	return this;
}

// When projectiles collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {

        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {
				this->AttemptToDropAnItem(gameModel);
			}
			break;

		case Projectile::CollateralBlockProjectile:
			// Both the collateral block and the item drop block will be destroyed in horrible, 
			// horrible gory glory
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
			this->AttemptToDropAnItem(gameModel);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a item drop block, unless it's frozen in an ice cube;
			// in that case, unfreeze a frozen item drop block
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
LevelPiece* ItemDropBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(gameModel != NULL);

	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	this->hitPointsBeforeNextDrop -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	
	if (this->hitPointsBeforeNextDrop <= 0) {
		// Reset the hit points for the next drop
		this->hitPointsBeforeNextDrop = ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP;

		// We now need to drop an item...
        gameModel->AddItemDrop(this->GetCenter(), this->nextDropItemType);
		this->ChangeToNextItemDropType(true);
	}

	return this;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* ItemDropBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	this->hitPointsBeforeNextDrop -= static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond()));
	
	if (this->hitPointsBeforeNextDrop <= 0) {
		// Reset the hit points for the next drop
		this->hitPointsBeforeNextDrop = ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP;

		// We now need to drop an item...
        gameModel->AddItemDrop(this->GetCenter(), this->nextDropItemType);
		this->ChangeToNextItemDropType(true);
	}

	return this;
}

// Update the nextDropItemType of this to be some random item type from the list of allowable item drops
void ItemDropBlock::ChangeToNextItemDropType(bool doEvent) {
	size_t randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->allowedItemDropTypes.size();
	this->nextDropItemType = this->allowedItemDropTypes[randomIdx];
	this->nextDropDisposition = GameItemFactory::GetInstance()->GetItemTypeDisposition(this->nextDropItemType);
	this->timeOfLastDrop = BlammoTime::GetSystemTimeInMillisecs();

	if (doEvent) {
		// EVENT: Item drop type changed
		GameEventManager::Instance()->ActionItemDropBlockItemChange(*this);
	}
}