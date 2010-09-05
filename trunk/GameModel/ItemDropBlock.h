/**
 * ItemDropBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ITEMDROPBLOCK_H__
#define __ITEMDROPBLOCK_H__

#include "LevelPiece.h"
#include "GameItem.h"


class ItemDropBlock : public LevelPiece {

public:
	ItemDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes, unsigned int wLoc, unsigned int hLoc);
	~ItemDropBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::ItemDrop;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// Tesla blocks can NEVER be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool UberballBlastsThrough() const {
		return false;	// Cannot pass through tesla blocks...
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	// You get no points for collisions with tesla blocks...
	int GetPointValueForCollision() {
		return 0;
	}

	// Tesla blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		return false;
	}

	bool ProjectilePassesThrough(Projectile* projectile) {
		return false;
	}

	LevelPiece* Destroy(GameModel* gameModel) {
		return this;
	}

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);

	const GameItem::ItemType& GetNextDropItemType() const;

private:
	static const float DAMAGE_UNTIL_ITEM_DROP;
	
	std::vector<GameItem::ItemType> allowedItemDropTypes;

	float hitPointsBeforeNextDrop;							// Hit points left that must be deminished (by a laser beam) before the next item drop
	GameItem::ItemType nextDropItemType;				// The next item type that will drop from this block

	void ChangeToNextItemDropType();
};

// Get what the next item drop will be
inline const GameItem::ItemType& ItemDropBlock::GetNextDropItemType() const {
	return this->nextDropItemType;
}

// Update the nextDropItemType of this to be some random item type from the list of allowable item drops
inline void ItemDropBlock::ChangeToNextItemDropType() {
	size_t randomIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->allowedItemDropTypes.size();
	this->nextDropItemType = this->allowedItemDropTypes[randomIdx];
}

#endif // __ITEMDROPBLOCK_H__