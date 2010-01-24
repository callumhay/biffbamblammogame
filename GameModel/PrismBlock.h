/**
 * PrismBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PRISMBLOCK_H__
#define __PRISMBLOCK_H__

#include "LevelPiece.h"

/**
 * Represents a prism block in the game. Prism blocks are not destroyable,
 * they are instead used to refract and reflect other effects. For example projectiles
 * and beams.
 */
class PrismBlock : public LevelPiece {

public:
	PrismBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc) {}
	virtual ~PrismBlock() {}

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Prism;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	virtual bool IsNoBoundsPieceType() const {
		return false;
	}

	// Prism blocks can NEVER be destroyed...
	virtual bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	virtual bool CanBeDestroyed() const {
		return false;
	}

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	virtual bool UberballBlastsThrough() const {
		return false;	// Cannot pass through prism blocks...
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	virtual bool GhostballPassesThrough() const {
		return true; // Ghost ball can float through them, why not?
	}

	// You get no points for collisions with prism blocks...
	virtual int GetPointValueForCollision() {
		return 0;
	}

	// All projectiles pass through the prism block
	// Returns: true.
	virtual bool ProjectilePassesThrough(Projectile* projectile) {
		return true;
	}

	// Prism blocks reflect and refract light.
	// Returns: true
	bool IsLightReflectorRefractor() const {
		return true;
	}

	LevelPiece* Destroy(GameModel* gameModel){
		return this;
	};

	virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
													  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
														const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
														const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	// Doesn't matter if a ball collides with a prism block, it does nothing to the block.
	LevelPiece* CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
		return this->Destroy(gameModel);
	}

	virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	virtual std::list<Collision::Ray2D> GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const;

};
#endif // __PRISMBLOCK_H__