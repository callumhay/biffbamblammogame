/**
 * RegenBlock.h
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

#ifndef __REGENBLOCK_H__
#define __REGENBLOCK_H__

#include "LevelPiece.h"
#include "Beam.h"

class RegenBlock : public LevelPiece {
public:
    static const int MAX_LIFE_POINTS = 100;  // Starting life points given to a regen block

	RegenBlock(bool hasInfiniteLife, unsigned int wLoc, unsigned int hLoc);
	~RegenBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Regen;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const;

	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	bool MustBeDestoryedToEndLevel() const {
        return !this->HasInfiniteLife();
	}
	bool CanBeDestroyedByBall() const {
		return true;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

	bool BallBlastsThrough(const GameBall& b) const;

	bool GhostballPassesThrough() const {
		return !this->HasInfiniteLife();
	}

	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers
		return this->HasStatus(LevelPiece::IceCubeStatus);
	}
    
	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses); 

    bool HasInfiniteLife() const;
    float GetCurrentLifePercent() const;
    int GetCurrentLifePercentInt() const;

    void Regen(double dT);

private:
    static const int INFINITE_LIFE_POINTS = -1;
	static const int POINTS_ON_BLOCK_DESTROYED  = 500;	// Points obtained when you destory a regen block
    static const float REGEN_LIFE_POINTS_PER_SECOND;    // Number of points this block regenerates per-second

    static const int MAX_FIREGLOB_COUNTDOWNS_WITHOUT_DROP = 3;

    float currLifePoints; // Amount of life that this block currently has

	double fireGlobDropCountdown;	       // Used to keep track of when to drop the next fire glob (when the block is on fire)
    int numFireGlobCountdownsWithoutDrop;  // Tracks the number of times that the fire glob countdown has happened

    LevelPiece* HurtPiece(float damage, GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    DISALLOW_COPY_AND_ASSIGN(RegenBlock);
};

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
inline LevelPiece* RegenBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);

	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}
    return this->HurtPiece(dT * beamSegment->GetDamagePerSecond(), gameModel, LevelPiece::LaserBeamDestruction);
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
inline LevelPiece* RegenBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
    return this->HurtPiece(dT * paddle.GetShieldDamagePerSecond(), gameModel, LevelPiece::PaddleShieldDestruction);
}

inline void RegenBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                     const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                     const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                     const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
    if (this->HasInfiniteLife()) {
        LevelPiece::UpdateSolidRectBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
    }
    else {
        LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
    }
}

// Get whether or not this regen block has infinite life
inline bool RegenBlock::HasInfiniteLife() const {
    return (this->currLifePoints == INFINITE_LIFE_POINTS);
}

/**
 * Gets the life as a percentage value [0, 100].
 */
inline float RegenBlock::GetCurrentLifePercent() const {
    assert(this->currLifePoints >= 0 && this->currLifePoints <= MAX_LIFE_POINTS);
    return this->currLifePoints;
}

inline int RegenBlock::GetCurrentLifePercentInt() const {
    return static_cast<int>(ceilf(this->GetCurrentLifePercent()));
}

#endif // __REGENBLOCK_H__