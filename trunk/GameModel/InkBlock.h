#ifndef __INKBLOCK_H__
#define __INKBLOCK_H__

#include "LevelPiece.h"

class InkBlock : public LevelPiece {
public:
	InkBlock(unsigned int wLoc, unsigned int hLoc);
	virtual ~InkBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Ink;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// Doesn't need to be destroyed to end the level - it's more just an
	// obstruction to make it harder if it's hit
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return true;
	}

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool UberballBlastsThrough() const {
		return true;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	// Obtain the point value for a collision with this block.
	// Returns: point value.
	int GetPointValueForCollision() {
		return POINTS_ON_BLOCK_DESTROYED;
	}

	// Particles do not pass through ink blocks.
	// Return: false.
	bool ProjectilePassesThrough(Projectile* projectile) {
		return false;
	}

	// Ink blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		return false;
	}

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);

protected:
	static const int POINTS_ON_BLOCK_DESTROYED = 15;	// Points obtained when you destory an ink block
	static const int PIECE_STARTING_LIFE_POINTS = 80;	// Starting life points given to a ink block

	float currLifePoints;	// Current life points of this block

};
#endif