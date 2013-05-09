/**
 * NoEntryBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelPiece.h"

#ifndef __NOENTRYBLOCK_H__
#define __NOENTRYBLOCK_H__

class NoEntryBlock : public LevelPiece {
public:

	NoEntryBlock(unsigned int wLoc, unsigned int hLoc);
	~NoEntryBlock();

	LevelPieceType GetType() const { return LevelPiece::NoEntry; }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const { UNUSED_PARAMETER(b); return true; }
	bool IsNoBoundsPieceType() const { return false; }
	bool BallBouncesOffWhenHit() const { return true; }
	bool MustBeDestoryedToEndLevel() const { return false; }
	bool CanBeDestroyedByBall() const { return false; }
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const { return false; }
    bool BallBlastsThrough(const GameBall& b) const { UNUSED_PARAMETER(b); return false; }
	bool GhostballPassesThrough() const { return false; }
    bool ProjectilePassesThrough(const Projectile* projectile) const;
	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
    }

    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeUntilCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

    void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

private:
    static const int POINTS_ON_BLOCK_DESTROYED = 100;

    DISALLOW_COPY_AND_ASSIGN(NoEntryBlock);
};

inline bool NoEntryBlock::SecondaryCollisionCheck(double dT, const GameBall& ball) const {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(ball);

    return true;
}

inline bool NoEntryBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                        Collision::LineSeg2D& collisionLine,
                                        double& timeUntilCollision) const {

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), 
                                n, collisionLine, timeUntilCollision);
}

inline bool NoEntryBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    UNUSED_PARAMETER(ray);
    UNUSED_PARAMETER(rayT);
    if (this->IsLightReflectorRefractor()) {
		return Collision::IsCollision(ray, this->GetAABB(), rayT);
	}
    else {
        return false;
    }
}

inline bool NoEntryBlock::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
	return this->bounds.CollisionCheck(boundingLines);
}

inline bool NoEntryBlock::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
    return Collision::IsCollision(this->GetAABB(), c);
}

inline void NoEntryBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir,
                                                    std::list<Collision::Ray2D>& rays) const {

	// If this piece is frozen then there are reflection/refraction rays...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		this->GetIceCubeReflectionRefractionRays(hitPoint, impactDir, rays);
	}
	else {
		// The default behaviour is to just let the ray pass through
        rays.push_back(Collision::Ray2D(hitPoint, impactDir));
	}
}

inline bool NoEntryBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

#endif // __NOENTRYBLOCK_H__