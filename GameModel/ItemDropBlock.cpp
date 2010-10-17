/**
 * ItemDropBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "ItemDropBlock.h"
#include "GameModel.h"
#include "PaddleLaserBeam.h"
#include "GameItemFactory.h"

// Amount of damage the block will take before dropping an item while being hit by a beam
const float ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP = 150.0f;
// Amount of time (in milliseconds) that must be waited until another item drop is allowed (for things other than beam damage)
const unsigned long ItemDropBlock::DISABLE_DROP_TIME = 750;

ItemDropBlock::ItemDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), allowedItemDropTypes(droppableItemTypes), hitPointsBeforeNextDrop(ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP),
timeOfLastDrop(0) {
	assert(!droppableItemTypes.empty());

	// Prepare for the next random item...
	this->ChangeToNextItemDropType(false);
}

ItemDropBlock::~ItemDropBlock() {
}

// When balls collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	if ((BlammoTime::GetSystemTimeInMillisecs() - this->timeOfLastDrop) >= ItemDropBlock::DISABLE_DROP_TIME) {
		// Drop an item...
		gameModel->AddItemDrop(*this, this->nextDropItemType);
		this->ChangeToNextItemDropType(true);
	}

	// Tell the ball that it collided with this block
	ball.SetLastPieceCollidedWith(this);

	return this;
}

// When projectiles collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	UNUSED_PARAMETER(projectile);

	if ((BlammoTime::GetSystemTimeInMillisecs() - this->timeOfLastDrop) >= ItemDropBlock::DISABLE_DROP_TIME) {
		// Drop an item...
		gameModel->AddItemDrop(*this, this->nextDropItemType);
		this->ChangeToNextItemDropType(true);
	}

	return this;
}

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
LevelPiece* ItemDropBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(gameModel != NULL);
	this->hitPointsBeforeNextDrop -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	
	if (this->hitPointsBeforeNextDrop <= 0) {
		// Reset the hit points for the next drop
		this->hitPointsBeforeNextDrop = ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP;

		// We now need to drop an item...
		gameModel->AddItemDrop(*this, this->nextDropItemType);
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
		gameModel->AddItemDrop(*this, this->nextDropItemType);
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