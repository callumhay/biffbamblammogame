#ifndef __INKBLOCK_H__
#define __INKBLOCK_H__

#include "LevelPiece.h"

class InkBlock : public LevelPiece {
protected:
	static const int POINTS_ON_BLOCK_DESTROYED = 15;		// Points obtained when you destory an ink block

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

	// Obtain the point value for a collision with this breakable block.
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
	virtual bool IsLightReflectorRefractor() const {
		return false;
	}

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
};
#endif