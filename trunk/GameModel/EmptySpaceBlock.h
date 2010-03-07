#ifndef __EMPTYSPACEBLOCK_H__
#define __EMPTYSPACEBLOCK_H__

#include "LevelPiece.h"

class EmptySpaceBlock : public LevelPiece {

public:
	EmptySpaceBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	virtual ~EmptySpaceBlock() {}

	LevelPieceType GetType() const { 
		return LevelPiece::Empty;
	}

	// Empty blocks have no bounds... they don't exist as physical entities
	bool IsNoBoundsPieceType() const {
		return true;
	}

	// Empty spaces don't exist and cannot really be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return false;
	}

	// Any type of ball can blast through an empty space...
	bool UberballBlastsThrough() const {
		return true;
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

	// Collision related stuffs
	LevelPiece* Destroy(GameModel* gameModel){
		return this;
	};	
	
	// All projectiles pass through empty space
	// Returns: true.
	bool ProjectilePassesThrough(Projectile* projectile) {
		return true;
	}

	// Empty blocks do not reflect or refract light.
	// Returns: false
	virtual bool IsLightReflectorRefractor() const {
		return false;
	}

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
			// Empty spaces don't have bounds...
			this->bounds.Clear();
	};

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball) {
		// Nothing happens when there's nothing to collide with...
		return this->Destroy(gameModel);
	}

	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
		// Nothing happens when there's nothing to collide with...
		return this->Destroy(gameModel);
	}
};
#endif