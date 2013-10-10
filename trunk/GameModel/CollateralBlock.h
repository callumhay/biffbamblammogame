/**
 * CollateralBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __COLLATERALBLOCK_H__
#define __COLLATERALBLOCK_H__

#include "LevelPiece.h"

class CollateralBlockProjectile;

class CollateralBlock : public LevelPiece {
public:
	static const float COLLATERAL_FALL_SPEED;	// Speed that the collateral block falls at in units / second

	CollateralBlock(unsigned int wLoc, unsigned int hLoc);
	~CollateralBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Collateral;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        return !this->BallBlastsThrough(b) && 
               ((b.GetBallType() & GameBall::IceBall) == GameBall::IceBall) ||
               ((b.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
    }

	// Collateral blocks have bounds and the ball bounces off them...
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// Collateral blocks don't need to be destroyed to end a level, but they can be useful.. or painful depending
	// on how they are handled once they start barrelling downwards...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return true;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return false;
	}

	// Ghost ball can pass through
	bool GhostballPassesThrough() const {
		return true;
	}

	// Light beams will extinguish on contact
	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

	void Tick(double dT, const GameModel& model, CollateralBlockProjectile& collateralProjectile);

	enum CollateralBlockState { InitialState, WarningState, CollateralDamageState };
	CollateralBlock::CollateralBlockState GetState() const;
	float GetCurrentRotation() const;
	void TeleportToNewPosition(const Point2D& pos);

private:
	static const double WARNING_TIME_MIN;			// Minimum warning time before collateral damage mode
	static const double WARNING_TIME_MAX;			// Maximum warning time before collateral damage mode
	static const float ROTATION_SPEED;

	static const int PIECE_STARTING_LIFE_POINTS = 100;	// Starting life points given to a breakable block

	float currLifePoints;	// Current life points of this block

	CollateralBlockState currState;

	double currTimeElapsedSinceHit;	// Currently elapsed time since switching to the "WarningState"
	double timeUntilCollateralDmg;	// Total time between the "InitialState" and the "CollateralDamageState"
	float currentRotation;					// Rotation about the local x-axis as the block falls down in the collateral damage state
	float rotationSgn;							// Specify if the rotation is CW/CCW

	LevelPiece* Detonate(GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(CollateralBlock);
};

inline bool CollateralBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

inline CollateralBlock::CollateralBlockState CollateralBlock::GetState() const {
	return this->currState;
}

inline float CollateralBlock::GetCurrentRotation() const {
	return this->currentRotation;
}

inline void CollateralBlock::TeleportToNewPosition(const Point2D& pos) {
	this->center = pos;
}

#endif // __COLLATERALBLOCK_H__