#ifndef __SOLIDBLOCK_H__
#define __SOLIDBLOCK_H__

#include "LevelPiece.h"

class SolidBlock : public LevelPiece {

public:
	SolidBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	virtual ~SolidBlock() {}

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Solid;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	virtual bool IsNoBoundsPieceType() const {
		return false;
	}

	// Solid blocks can NEVER be destroyed...
	virtual bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	virtual bool CanBeDestroyed() const {
		return false;
	}

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	virtual bool UberballBlastsThrough() const {
		return false;	// Cannot pass through solid blocks...
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	virtual bool GhostballPassesThrough() const {
		return false; // Cannot pass through solid blocks... 
	}

	// You get no points for collisions with solid blocks...
	virtual int GetPointValueForCollision() {
		return 0;
	}

	virtual LevelPiece* Destroy(GameModel* gameModel){
		return this;
	};

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor);
	
	// Doesn't matter if a ball collides with solid block, it does nothing to the block.
	virtual LevelPiece* BallCollisionOccurred(GameModel* gameModel, const GameBall& ball) {
		return this->Destroy(gameModel);
	}
};
#endif