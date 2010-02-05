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

	// Empty blocks have no bounds... they don't exist as physical entities
	bool IsNoBoundsPieceType() const {
		return false;
	}

	// Empty spaces don't exist and cannot really be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return true;
	}

	// Any type of ball can blast through an empty space...
	bool UberballBlastsThrough() const {
		return false;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	// You get no points for empty spaces...
	int GetPointValueForCollision() {
		return 0;
	}

	// No projectiles pass through bomb blocks
	// Returns: false.
	bool ProjectilePassesThrough(Projectile* projectile) {
		return false;
	}

	// Bomb blocks do not reflect or refract light
	// Returns: false
	bool IsLightReflectorRefractor() const {
		return false;
	}

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel);
	LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);

protected:
	static const int PIECE_STARTING_LIFE_POINTS = 120;	// Starting life points given to a bomb block
	float currLifePoints;	// Current life points of this block

};
#endif