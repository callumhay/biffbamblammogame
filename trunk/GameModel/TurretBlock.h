/**
 * TurretBlock.h
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

#ifndef __TURRETBLOCK_H__
#define __TURRETBLOCK_H__

#include "LevelPiece.h"

class TurretBlock : public LevelPiece {
public:
	TurretBlock(unsigned int wLoc, unsigned int hLoc, float life);
    virtual ~TurretBlock();

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const;
	bool IsNoBoundsPieceType() const { return false; }
	bool BallBouncesOffWhenHit() const { return true; }
	bool MustBeDestoryedToEndLevel() const { return false; }
	bool CanBeDestroyedByBall() const { return true; }
    bool CanChangeSelfOrOtherPiecesWhenHit() const { return true; }
    bool BallBlastsThrough(const GameBall& b) const { UNUSED_PARAMETER(b); return false; }
    bool GhostballPassesThrough() const { return false; }

    bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		return this->HasStatus(LevelPiece::IceCubeStatus);
	}

	bool ProjectileIsDestroyedOnCollision(const Projectile* projectile) const;

    bool IsAIPiece() const { return true; } // All turrets are AI driven

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses); 

    float GetHealthPercent() const;
    float GetLifePercentForOneMoreBallHit() const;

protected:
    const float startingLifePoints;
    float currLifePoints;

    bool IsDead() const { return this->currLifePoints <= 0; }
    LevelPiece* DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method);

private:
    DISALLOW_COPY_AND_ASSIGN(TurretBlock);
};

inline bool TurretBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline void TurretBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

inline float TurretBlock::GetHealthPercent() const {
    return this->currLifePoints / this->startingLifePoints;
}

inline float TurretBlock::GetLifePercentForOneMoreBallHit() const {
    return GameModelConstants::GetInstance()->DEFAULT_DAMAGE_ON_BALL_HIT / this->startingLifePoints;
}

inline LevelPiece* TurretBlock::DiminishPiece(float dmgAmount, GameModel* model,
                                              const LevelPiece::DestructionMethod& method) {
    this->currLifePoints -= dmgAmount;
    if (this->currLifePoints <= 0.0f) {
        this->currLifePoints = 0.0f;
        return this->Destroy(model, method);
    }
    return this;
}

#endif // __TURRETBLOCK_H__