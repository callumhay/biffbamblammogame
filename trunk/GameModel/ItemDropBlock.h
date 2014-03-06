/**
 * ItemDropBlock.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return true;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const;
	bool IsNoBoundsPieceType() const;
	bool BallBouncesOffWhenHit() const;
	bool MustBeDestoryedToEndLevel() const;
	bool CanBeDestroyedByBall() const;
    bool CanChangeSelfOrOtherPiecesWhenHit() const;
	bool BallBlastsThrough(const GameBall& b) const;
	bool GhostballPassesThrough() const;
	bool IsLightReflectorRefractor() const;
	bool ProjectilePassesThrough(const Projectile* projectile) const;

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    };

    void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

    void Triggered(GameModel* gameModel);

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

	void AttemptToDropAnItem(GameModel* gameModel);
	const GameItem::ItemType& GetNextItemDropType() const;
	const GameItem::ItemDisposition& GetNextDropItemDisposition() const;

    bool GetHasSparkleEffect() const;

private:
	static const float DAMAGE_UNTIL_ITEM_DROP;
	static const unsigned long DISABLE_DROP_TIME;

	std::vector<GameItem::ItemType> allowedItemDropTypes;

	float hitPointsBeforeNextDrop;                  // Hit points left that must be diminished (by a laser beam) before the next item drop
	int nextDropItemTypeIdx;                        // The index look-up within allowedItemDropTypes for the next item type that will drop from this block
	GameItem::ItemDisposition nextDropDisposition;	// The disposition (good/neutral/bad) of the next item type that will drop from this block

	unsigned long timeOfLastDrop;	// Amount of time since the last item drop

    bool hasSparkleEffect;

	void ChangeToNextItemDropType(bool doEvent);
    void SetNextItemDropTypeIndex(int index);

    DISALLOW_COPY_AND_ASSIGN(ItemDropBlock);
};

inline LevelPiece::LevelPieceType ItemDropBlock::GetType() const { 
	return LevelPiece::ItemDrop;
}

inline bool ItemDropBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    UNUSED_PARAMETER(b);
    return true;
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
inline bool ItemDropBlock::CanChangeSelfOrOtherPiecesWhenHit() const {
    return false;
}

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
inline bool ItemDropBlock::BallBlastsThrough(const GameBall& b) const {
    UNUSED_PARAMETER(b);
	return false;
}

// Whether or not the ghost ball can just pass through this block.
// Returns: true if it can, false otherwise.
inline bool ItemDropBlock::GhostballPassesThrough() const {
	return false;
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

inline bool ItemDropBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline void ItemDropBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    // Check to see if there's something directly below this piece, if there is then we don't do the sparkle effect
    if (bottomNeighbor != NULL) {
        this->hasSparkleEffect = bottomNeighbor->IsNoBoundsPieceType();
    }

    LevelPiece::UpdateSolidRectBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

// Get what the next item drop will be
inline const GameItem::ItemType& ItemDropBlock::GetNextItemDropType() const {
	return this->allowedItemDropTypes[this->nextDropItemTypeIdx];
}

inline const GameItem::ItemDisposition& ItemDropBlock::GetNextDropItemDisposition() const {
	return this->nextDropDisposition;
}

inline bool ItemDropBlock::GetHasSparkleEffect() const {
    return this->hasSparkleEffect;
}

inline void ItemDropBlock::SetNextItemDropTypeIndex(int index) {
    assert(index >= 0 && index < static_cast<int>(this->allowedItemDropTypes.size()));
    this->nextDropItemTypeIdx = index;
    this->nextDropDisposition = GameItemFactory::GetInstance()->GetItemTypeDisposition(this->GetNextItemDropType());
}

#endif // __ITEMDROPBLOCK_H__