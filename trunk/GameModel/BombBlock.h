#ifndef __BOMBBLOCK_H__
#define __BOMBBLOCK_H__

#include "LevelPiece.h"

class BombBlock : public LevelPiece {

public:
	BombBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	virtual ~BombBlock() {}

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Bomb;
	}

	// Empty blocks have no bounds... they don't exist as physical entities
	virtual bool IsNoBoundsPieceType() const {
		return false;
	}

	// Empty spaces don't exist and cannot really be destroyed...
	virtual bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	virtual bool CanBeDestroyed() const {
		return true;
	}

	// Any type of ball can blast through an empty space...
	virtual bool UberballBlastsThrough() const {
		return false;
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	virtual bool GhostballPassesThrough() const {
		return true;
	}

	// You get no points for empty spaces...
	virtual int GetPointValueForCollision() {
		return 0;
	}

	// Collision related stuffs
	virtual LevelPiece* Destroy(GameModel* gameModel);
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball);
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, const Projectile& projectile);

};
#endif