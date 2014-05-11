/**
 * AlwaysDropBlock.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __ALWAYSDROPBLOCK_H__
#define __ALWAYSDROPBLOCK_H__

#include "LevelPiece.h"
#include "Beam.h"
#include "GameItem.h"

class AlwaysDropBlock : public LevelPiece {
public:
	AlwaysDropBlock(const std::vector<GameItem::ItemType>& droppableItemTypes,
        unsigned int wLoc, unsigned int hLoc);
	~AlwaysDropBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::AlwaysDrop;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        return !this->BallBlastsThrough(b) && (b.HasBallType(GameBall::IceBall) || b.HasBallType(GameBall::FireBall));
    }

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return true;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

    // Whether or not the ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool BallBlastsThrough(const GameBall& b) const {
        // The ball may blast through ONLY IF the piece is green, the ball is uber and NOT on fire / icy
        return (b.HasBallType(GameBall::UberBall) && !b.HasBallType(GameBall::IceBall) && !b.HasBallType(GameBall::FireBall));
	}

    // Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

    bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	// Breakable blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

	// Collision related stuffs
    void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

    const GameItem::ItemType& GetNextDropItemType() const;
	const GameItem::ItemDisposition& GetNextDropItemDisposition() const;

private:
    static const int PIECE_STARTING_LIFE_POINTS = 100;
    static const int POINTS_ON_BLOCK_DESTROYED  = 100;

    float currLifePoints;

    GameItem::ItemType nextDropItemType;          
    GameItem::ItemDisposition nextDropDisposition;

    LevelPiece* EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    DISALLOW_COPY_AND_ASSIGN(AlwaysDropBlock);
};

inline int AlwaysDropBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return AlwaysDropBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
inline LevelPiece* AlwaysDropBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment,
                                                      GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);

	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}
    return this->EatAwayAtPiece(dT, beamSegment->GetDamagePerSecond(),
        gameModel, LevelPiece::LaserBeamDestruction);
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
inline LevelPiece* AlwaysDropBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle,
                                                             GameModel* gameModel) {
	assert(gameModel != NULL);
    return this->EatAwayAtPiece(dT, paddle.GetShieldDamagePerSecond(),
        gameModel, LevelPiece::PaddleShieldDestruction);
}

// Get what the next item drop will be
inline const GameItem::ItemType& AlwaysDropBlock::GetNextDropItemType() const {
	return this->nextDropItemType;
}

inline const GameItem::ItemDisposition& AlwaysDropBlock::GetNextDropItemDisposition() const {
	return this->nextDropDisposition;
}

inline void AlwaysDropBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                          const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                          const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                          const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}


#endif // __ALWAYSDROPBLOCK_H__