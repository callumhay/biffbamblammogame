/**
 * CollateralBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

	// Uberball doesn't blast through
	bool UberballBlastsThrough() const {
		return false;
	}

	// Ghost ball can pass through
	bool GhostballPassesThrough() const {
		return true;
	}

	// Obtain the point value for a collision with this block.
	int GetPointValueForCollision() {
		return CollateralBlock::POINTS_ON_BLOCK_DESTROYED;
	}

	// Light beams will extinguish on contact
	bool IsLightReflectorRefractor() const {
		return false;
	}

	bool ProjectilePassesThrough(Projectile* projectile) const;

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	void Tick(double dT, CollateralBlockProjectile& collateralProjectile);
	enum CollateralBlockState { InitialState, WarningState, CollateralDamageState };
	CollateralBlock::CollateralBlockState GetState() const;
	float GetCurrentRotation() const;
	void TeleportToNewPosition(const Point2D& pos);

private:
	static const double WARNING_TIME_MIN;			// Minimum warning time before collateral damage mode
	static const double WARNING_TIME_MAX;			// Maximum warning time before collateral damage mode
	static const float ROTATION_SPEED;

	static const int PIECE_STARTING_LIFE_POINTS = 100;	// Starting life points given to a breakable block
	static const int POINTS_ON_BLOCK_DESTROYED	= 10;		// Points obtained when you destory a breakable block

	float currLifePoints;	// Current life points of this block

	CollateralBlockState currState;

	double currTimeElapsedSinceHit;	// Currently elapsed time since switching to the "WarningState"
	double timeUntilCollateralDmg;	// Total time between the "InitialState" and the "CollateralDamageState"
	float currentRotation;					// Rotation about the local x-axis as the block falls down in the collateral damage state
	float rotationSgn;							// Specify if the rotation is CW/CCW

	LevelPiece* Detonate(GameModel* gameModel);

};

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