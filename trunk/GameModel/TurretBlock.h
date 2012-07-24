/**
 * TurretBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TURRETBLOCK_H__
#define __TURRETBLOCK_H__

#include "LevelPiece.h"

class TurretBlock : public LevelPiece {
public:
	TurretBlock(unsigned int wLoc, unsigned int hLoc, float life);
    virtual ~TurretBlock();

	bool IsNoBoundsPieceType() const { return false; }
	bool BallBouncesOffWhenHit() const { return true; }
	bool MustBeDestoryedToEndLevel() const { return false; }
	bool CanBeDestroyedByBall() const { return true; }
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const { return true; }
    bool BallBlastsThrough(const GameBall& b) const { UNUSED_PARAMETER(b); return false; }
    bool GhostballPassesThrough() const { return false; }

    bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		return this->HasStatus(LevelPiece::IceCubeStatus);
	}

	bool ProjectileIsDestroyedOnCollision(Projectile* projectile) const;

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

protected:
    const float startingLifePoints;
    float currLifePoints;

    bool IsDead() const { return this->currLifePoints <= 0; }
    LevelPiece* DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method);

    float GetBallDamage() const;

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

inline float TurretBlock::GetHealthPercent() const {
    return this->currLifePoints / this->startingLifePoints;
}

inline float TurretBlock::GetBallDamage() const {
    return this->startingLifePoints / 6.0f;
}

inline LevelPiece* TurretBlock::DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method) {
    this->currLifePoints -= dmgAmount;
    if (this->currLifePoints <= 0.0f) {
        return this->Destroy(model, method);
    }
    return this;
}

#endif // __TURRETBLOCK_H__