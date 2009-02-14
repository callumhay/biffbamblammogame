#ifndef __EMPTYSPACEBLOCK_H__
#define __EMPTYSPACEBLOCK_H__

#include "LevelPiece.h"

class EmptySpaceBlock : public LevelPiece {

public:
	EmptySpaceBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	virtual ~EmptySpaceBlock() {}

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Empty;
	}

	// Empty blocks have no bounds... they don't exist as physical entities
	virtual bool IsNoBoundsPieceType() const {
		return true;
	}

	// Empty spaces don't exist and cannot really be destroyed...
	virtual bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	virtual bool CanBeDestroyed() const {
		return false;
	}

	// Any type of ball can blast through an empty space...
	virtual bool UberballBlastsThrough() const {
		return true;
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
	virtual LevelPiece* Destroy(GameModel* gameModel){
		return this;
	};	
	
	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
		const LevelPiece* rightNeightbor, const LevelPiece* topNeighbor) {
			// Empty spaces don't have bounds...
			this->bounds.Clear();
	};

	virtual LevelPiece* BallCollisionOccurred(GameModel* gameModel, const GameBall& ball) {
		// Nothing happens when there's nothing to collide with...
		return this->Destroy(gameModel);
	}

};
#endif