/**
 * InkBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERTURRETBLOCK_H__
#define __LASERTURRETBLOCK_H__

#include "LevelPiece.h"

class LaserTurretBlock : public LevelPiece {
public:
	LaserTurretBlock(unsigned int wLoc, unsigned int hLoc);
	~LaserTurretBlock();

	LevelPieceType GetType() const { return LevelPiece::LaserTurret; }

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

	bool ProjectilePassesThrough(Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);


	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses); 

private:
    static const int POINTS_ON_BLOCK_DESTROYED  = 800;
    static const int PIECE_STARTING_LIFE_POINTS = 500;
    static const float BALL_DAMAGE_AMOUNT;

    float currLifePoints;

    LevelPiece* DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method);
    bool IsDead() const { return this->currLifePoints <= 0; }

    DISALLOW_COPY_AND_ASSIGN(LaserTurretBlock);
};

inline bool LaserTurretBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

#endif // __LASERTURRETBLOCK_H__