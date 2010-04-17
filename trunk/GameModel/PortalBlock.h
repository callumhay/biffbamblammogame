#ifndef __PORTALBLOCK_H__
#define __PORTALBLOCK_H__

#include "LevelPiece.h"

/**
 * Represents the model for a portal block - the piece that can teleport the ball
 * and various other things between itself and its sibling portal block.
 */
class PortalBlock : public LevelPiece {
public:
	PortalBlock(unsigned int wLoc, unsigned int hLoc, PortalBlock* sibling);
	~PortalBlock();

	const PortalBlock* GetSiblingPortal() const {
		return this->sibling;
	}
	PortalBlock* GetSiblingPortal() {
		return this->sibling;
	}
	void SetSiblingPortal(PortalBlock* sibling) {
		assert(sibling != NULL);
		this->sibling = sibling;
	}

	LevelPieceType GetType() const { 
		return LevelPiece::Portal;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return true;
	}
	bool BallBouncesOffWhenHit() const {
		return false;
	}	

	// Doesn't need to be destroyed to end the level - it's more just an
	// obstruction to make it harder if it's hit
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return false;
	}

	// Whether or not the uber ball can just blast right through this block.
	bool UberballBlastsThrough() const {
		return true;
	}

	// Whether or not the ghost ball can just pass through this block.
	bool GhostballPassesThrough() const {
		return false;
	}

	// Obtain the point value for a collision with this block.
	int GetPointValueForCollision() {
		return 0;
	}

	// Particles pass through portal blocks.
	bool ProjectilePassesThrough(Projectile* projectile) {
		return true;
	}

	// Portal blocks change light...
	bool IsLightReflectorRefractor() const {
		return true;
	}

	// Portal blocks are never destroyed.
	LevelPiece* Destroy(GameModel* gameModel) {
		return this;
	}

	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const Collision::AABB2D& aabb) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines) const;
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	std::list<Collision::Ray2D> GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const;

	static void ResetPortalColourGenerator();
	static Colour GeneratePortalColour();

protected:
	PortalBlock* sibling;

	static bool portalGeneratorReset;

};

#endif // __PORTALBLOCK_H__