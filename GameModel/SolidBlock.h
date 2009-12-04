/**
 * SolidBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	// Doesn't matter if a ball collides with solid block, it does nothing to the block.
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
		return this->Destroy(gameModel);
	}
	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
		return this->Destroy(gameModel);
	}
};
#endif