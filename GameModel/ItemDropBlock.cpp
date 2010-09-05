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

// Amount of damage the block will take before dropping an item while being hit by a beam
const float ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP = 150.0f;

ItemDropBlock::ItemDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), allowedItemDropTypes(droppableItemTypes), hitPointsBeforeNextDrop(ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP) {
	assert(!droppableItemTypes.empty());

	// Prepare for the next random item...
	this->ChangeToNextItemDropType();
}

ItemDropBlock::~ItemDropBlock() {
}

// When balls collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	// Drop an item...
	gameModel->AddItemDrop(*this, this->nextDropItemType);
	this->ChangeToNextItemDropType();

	return this;
}

// When projectiles collide with the item drop block it causes an item to fall from the block
LevelPiece* ItemDropBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// Drop an item...
	gameModel->AddItemDrop(*this, this->nextDropItemType);
	this->ChangeToNextItemDropType();

	return this;
}

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
LevelPiece* ItemDropBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	this->hitPointsBeforeNextDrop -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	
	if (this->hitPointsBeforeNextDrop <= 0) {
		// Reset the hit points for the next drop
		this->hitPointsBeforeNextDrop = ItemDropBlock::DAMAGE_UNTIL_ITEM_DROP;

		// We now need to drop an item...
		gameModel->AddItemDrop(*this, this->nextDropItemType);
		this->ChangeToNextItemDropType();
	
	}

	return this;
}