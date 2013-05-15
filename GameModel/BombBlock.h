/**
 * BombBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
        return !this->BallBlastsThrough(b) && 
               ((b.GetBallType() & GameBall::IceBall) == GameBall::IceBall) ||
               ((b.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
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
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const {
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
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
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

#endif