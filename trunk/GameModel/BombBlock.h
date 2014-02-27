/**
 * BombBlock.h
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

#ifndef __BOMBBLOCK_H__
#define __BOMBBLOCK_H__

#include "LevelPiece.h"

class BombBlock : public LevelPiece {

public:
	BombBlock(unsigned int wLoc, unsigned int hLoc);
	~BombBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Bomb;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        return !this->BallBlastsThrough(b) && (b.HasBallType(GameBall::IceBall) || b.HasBallType(GameBall::FireBall));
    }

	// Bomb blocks have bounds...
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// Bomb blocks don't need to be destroyed to end a level, they're just useful
	// for making the level end faster... usually
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return true;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

	// Any type of ball can blast through an empty space...
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return false;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	bool ProjectilePassesThrough(const Projectile* projectile) const;

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

protected:
    static const int BOMB_DESTRUCTION_POINTS    = 50;
	static const int PIECE_STARTING_LIFE_POINTS = 120;	// Starting life points given to a bomb block
	float currLifePoints;	// Current life points of this block

};

inline bool BombBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline void BombBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                    const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                    const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                    const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}


#endif