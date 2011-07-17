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
#include "GameModel.h"

class ItemDropBlock : public LevelPiece {

public:
	ItemDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes, unsigned int wLoc, unsigned int hLoc);
	~ItemDropBlock();

	LevelPieceType GetType() const;
	bool IsNoBoundsPieceType() const;
	bool BallBouncesOffWhenHit() const;
	bool MustBeDestoryedToEndLevel() const;
	bool CanBeDestroyedByBall() const;
	bool BallBlastsThrough(const GameBall& b) const;
	bool GhostballPassesThrough() const;
	bool IsLightReflectorRefractor() const;
	bool ProjectilePassesThrough(Projectile* projectile) const;

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    };

    void Triggered(GameModel* gameModel);

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

	void AttemptToDropAnItem(GameModel* gameModel);
	const GameItem::ItemType& GetNextDropItemType() const;
	const GameItem::ItemDisposition& GetNextDropItemDisposition() const;

private:
	static const float DAMAGE_UNTIL_ITEM_DROP;
	static const unsigned long DISABLE_DROP_TIME;

	std::vector<GameItem::ItemType> allowedItemDropTypes;

	float hitPointsBeforeNextDrop;                  // Hit points left that must be deminished (by a laser beam) before the next item drop
	GameItem::ItemType nextDropItemType;            // The next item type that will drop from this block
	GameItem::ItemDisposition nextDropDisposition;	// The disposition (good/neutral/bad) of the next item type that will drop from this block

	unsigned long timeOfLastDrop;	// Amount of time since the last item drop

	void ChangeToNextItemDropType(bool doEvent);

    DISALLOW_COPY_AND_ASSIGN(ItemDropBlock);
};

inline LevelPiece::LevelPieceType ItemDropBlock::GetType() const { 
	return LevelPiece::ItemDrop;
}

// Is this piece one without any boundries (i.e., no collision surface/line)?
// Return: true if non-collider, false otherwise.
inline bool ItemDropBlock::IsNoBoundsPieceType() const {
	return false;
}
inline bool ItemDropBlock::BallBouncesOffWhenHit() const {
	return true;
}

// Item drop blocks can NEVER be destroyed...
inline bool ItemDropBlock::MustBeDestoryedToEndLevel() const {
	return false;
}
inline bool ItemDropBlock::CanBeDestroyedByBall() const {
	return false;
}

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
inline bool ItemDropBlock::BallBlastsThrough(const GameBall& b) const {
    UNUSED_PARAMETER(b);
	return false;	// Cannot pass through tesla blocks...
}

// Whether or not the ghost ball can just pass through this block.
// Returns: true if it can, false otherwise.
inline bool ItemDropBlock::GhostballPassesThrough() const {
	return true;
}

// Item drop blocks do not reflect or refract light.
// Returns: false
inline bool ItemDropBlock::IsLightReflectorRefractor() const {
	// When frozen in ice a block can reflect/refract lasers and the like
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return true;
	}
	return false;
}

inline void ItemDropBlock::Triggered(GameModel* gameModel) {
    // When triggered, an item drop block will try to drop an item
    this->AttemptToDropAnItem(gameModel);
}

inline bool ItemDropBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline void ItemDropBlock::AttemptToDropAnItem(GameModel* gameModel) {
	// Drop an item if the item drop timer allows it...
	if ((BlammoTime::GetSystemTimeInMillisecs() - this->timeOfLastDrop) >= ItemDropBlock::DISABLE_DROP_TIME) {
		gameModel->AddItemDrop(*this, this->nextDropItemType);
		this->ChangeToNextItemDropType(true);
	}
}

// Get what the next item drop will be
inline const GameItem::ItemType& ItemDropBlock::GetNextDropItemType() const {
	return this->nextDropItemType;
}

inline const GameItem::ItemDisposition& ItemDropBlock::GetNextDropItemDisposition() const {
	return this->nextDropDisposition;
}

#endif // __ITEMDROPBLOCK_H__