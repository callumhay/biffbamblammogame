/**
 * SolidBlock.h
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

#ifndef __SOLIDBLOCK_H__
#define __SOLIDBLOCK_H__

#include "LevelPiece.h"
#include "GameBall.h"

class SolidBlock : public LevelPiece {

public:
	SolidBlock(unsigned int wLoc, unsigned int hLoc);
	virtual ~SolidBlock();

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Solid;
	}

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return true;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        UNUSED_PARAMETER(b);
        return true;
    }

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// Solid blocks can NEVER be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return false;
    }

	// Whether or not the ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return false;	// Cannot pass through solid blocks...
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return false; // Cannot pass through solid blocks... 
	}

	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

	virtual bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
	                          const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

private:
    static const int POINTS_ON_BLOCK_DESTROYED = 100;
    DISALLOW_COPY_AND_ASSIGN(SolidBlock);
};

inline bool SolidBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline void SolidBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                     const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                     const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                     const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    LevelPiece::UpdateSolidRectBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

#endif