/**
* PrismBlock.h
*
* (cc) Creative Commons Attribution-Noncommercial 3.0 License
* Callum Hay, 2011
*
* You may not use this work for commercial purposes.
* If you alter, transform, or build upon this work, you may distribute the 
* resulting work only under the same or similar license to this one.
*/

#ifndef __PRISMBLOCK_H__
#define __PRISMBLOCK_H__

#include "LevelPiece.h"
#include "Projectile.h"

/**
* Represents a prism block in the game. Prism blocks are not destroyable,
* they are instead used to refract and reflect other effects. For example projectiles
* and beams.
*/
class PrismBlock : public LevelPiece {

public:
    static const float REFLECTION_REFRACTION_SPLIT_ANGLE;

    PrismBlock(unsigned int wLoc, unsigned int hLoc);
    virtual ~PrismBlock();

    virtual LevelPieceType GetType() const { 
        return LevelPiece::Prism;
    }

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return true;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
        UNUSED_PARAMETER(b);    
        return true;
    }

    // Is this piece one without any boundries (i.e., no collision surface/line)?
    // Return: true if non-collider, false otherwise.
    bool IsNoBoundsPieceType() const {
        return false;
    }
    bool BallBouncesOffWhenHit() const {
        return true;
    }

    // Prism blocks can NEVER be destroyed...
    bool MustBeDestoryedToEndLevel() const {
        return false;
    }
    bool CanBeDestroyedByBall() const {
        return false;
    }
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const {
        return false;
    }

    // Whether or not the uber ball can just blast right through this block.
    // Returns: true if it can, false otherwise.
    bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
        return false;	// Cannot pass through prism blocks...
    }

    // Whether or not the ghost ball can just pass through this block.
    // Returns: true if it can, false otherwise.
    bool GhostballPassesThrough() const {
        return true; // Ghost ball can float through them, why not?
    }

    bool ProjectilePassesThrough(const Projectile* projectile) const {
        return projectile->IsRefractableOrReflectable();
    }

    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0;
    };

    // Prism blocks reflect and refract light.
    // Returns: true
    bool IsLightReflectorRefractor() const {
        return true;
    }

    LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
    virtual void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
        const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
        const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
        const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

    // Doesn't matter if a ball collides with a prism block, it does nothing to the block.
    LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball) {
        UNUSED_PARAMETER(gameModel);

        // Tell the ball what the last piece it collided with was...
        ball.SetLastPieceCollidedWith(NULL);
        return this;
    }

    bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;

    virtual LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

    //protected:
    //	BoundingLines reflectRefractBounds;	// Bounds used for reflection/refraction of beams
};

inline bool PrismBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);
}

#endif // __PRISMBLOCK_H__