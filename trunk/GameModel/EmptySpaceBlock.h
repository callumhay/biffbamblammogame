/**
 * EmptySpaceBlock.h
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

#ifndef __EMPTYSPACEBLOCK_H__
#define __EMPTYSPACEBLOCK_H__

#include "LevelPiece.h"

class EmptySpaceBlock : public LevelPiece {

public:
	EmptySpaceBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	~EmptySpaceBlock() {}

	LevelPieceType GetType() const { 
		return LevelPiece::Empty;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        UNUSED_PARAMETER(b);
        return false;
    }

	// Empty blocks have no bounds... they don't exist as physical entities
	bool IsNoBoundsPieceType() const {
		return true;
	}
	bool BallBouncesOffWhenHit() const {
		return false;
	}

	// Empty spaces don't exist and cannot really be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return false;
    }

	// Any type of ball can blast through an empty space...
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return true;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method){
		UNUSED_PARAMETER(gameModel);
        UNUSED_PARAMETER(method);
		return this;
	};	
	
	// All projectiles pass through empty space
	// Returns: true.
	bool ProjectilePassesThrough(const Projectile* projectile) const {
		UNUSED_PARAMETER(projectile);
		return true;
	}
    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

	// Empty blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		return false;
	}

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
			UNUSED_PARAMETER(leftNeighbor);
			UNUSED_PARAMETER(bottomNeighbor);
			UNUSED_PARAMETER(rightNeighbor);
			UNUSED_PARAMETER(topNeighbor);
			UNUSED_PARAMETER(topRightNeighbor);
			UNUSED_PARAMETER(topLeftNeighbor);
			UNUSED_PARAMETER(bottomRightNeighbor);
			UNUSED_PARAMETER(bottomLeftNeighbor);

			// Empty spaces don't have bounds...
			this->bounds.Clear();
	};

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball) {
		UNUSED_PARAMETER(gameModel);
		UNUSED_PARAMETER(ball);
		// Nothing happens when there's nothing to collide with...
		return this;
	}

	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
		UNUSED_PARAMETER(gameModel);
		UNUSED_PARAMETER(projectile);
		// Nothing happens when there's nothing to collide with...
		return this;
	}
};

#endif