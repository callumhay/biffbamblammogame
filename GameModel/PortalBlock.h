/**
 * PortalBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const {
        return false;
    }

	// Whether or not the ball can just blast right through this block.
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		// All balls pass through the portal
        return true;
	}

	// Whether or not the ghost ball can just pass through this block.
	bool GhostballPassesThrough() const {
		return false;
	}

	// Particles pass through portal blocks.
	bool ProjectilePassesThrough(Projectile* projectile) const {
		UNUSED_PARAMETER(projectile);
		return true;
	}

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    }

	// Portal blocks change light...
	bool IsLightReflectorRefractor() const {
		return true;
	}

	// Portal blocks are never destroyed.
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
		UNUSED_PARAMETER(gameModel);
        UNUSED_PARAMETER(method);
		return this;
	}

	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

	static void ResetPortalColourGenerator();
	static const Colour& GeneratePortalColour();

protected:
    static const unsigned long TIME_BETWEEN_BALL_USES_IN_MILLISECONDS;
	PortalBlock* sibling;

	static bool portalGeneratorReset;
    unsigned long timeOfLastBallCollision;

    DISALLOW_COPY_AND_ASSIGN(PortalBlock);
};

inline bool PortalBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);	
}

inline bool PortalBlock::CollisionCheck(const BoundingLines& boundingLines,
                                        const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
	return this->bounds.CollisionCheck(boundingLines);
}


#endif // __PORTALBLOCK_H__