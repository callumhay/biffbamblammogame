/**
 * OneWayBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ONEWAYBLOCK_H__
#define __ONEWAYBLOCK_H__

#include "LevelPiece.h"
#include "GameBall.h"

class OneWayBlock : public LevelPiece {
public:

    enum OneWayDir { OneWayUp, OneWayDown, OneWayLeft, OneWayRight };
	OneWayBlock(const OneWayDir& dir, unsigned int wLoc, unsigned int hLoc);
	~OneWayBlock();

    static bool ConvertCharToOneWayDir(const char& oneWayChar, OneWayBlock::OneWayDir& oneWayDirEnum);

    const OneWayDir& GetDirType() const {
        return this->dirType;
    }

	LevelPieceType GetType() const { 
		return LevelPiece::OneWay;
	}

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}

	// One-way blocks can NEVER be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}

	// Whether or not the ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool BallBlastsThrough(const GameBall& b) const;

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return false; // Cannot pass through one-way blocks all the time... 
	}

	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

	bool ProjectilePassesThrough(Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

private:
    static const int POINTS_ON_BLOCK_DESTROYED = 700;

    OneWayDir dirType;
    Vector2D oneWayDir;

    bool IsGoingTheOneWay(const Vector2D& v) const;

    DISALLOW_COPY_AND_ASSIGN(OneWayBlock);
};

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
inline bool OneWayBlock::BallBlastsThrough(const GameBall& b) const {
    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return this->IsGoingTheOneWay(b.GetDirection());
}

inline bool OneWayBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                        Collision::LineSeg2D& collisionLine,
                                        double& timeSinceCollision) const {

    // Collisions only occur if the ball is not going 'the one way' or if the piece is
    // frozen in ice
    if (this->IsGoingTheOneWay(ball.GetDirection()) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), 
                                n, collisionLine, timeSinceCollision);
}

inline bool OneWayBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    if (this->IsGoingTheOneWay(ray.GetUnitDirection()) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

inline bool OneWayBlock::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const {
    if (this->IsGoingTheOneWay(velDir) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return this->bounds.CollisionCheck(boundingLines);
}

inline bool OneWayBlock::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    if (this->IsGoingTheOneWay(velDir) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return Collision::IsCollision(this->GetAABB(), c);
}

inline bool OneWayBlock::StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses) {
	UNUSED_PARAMETER(dT);
	UNUSED_PARAMETER(gameModel);
	assert(gameModel != NULL);

	removedStatuses = static_cast<int32_t>(LevelPiece::NormalStatus);
	return false;
}

// Returns: true if the given vector may pass through this block, false otherwise.
inline bool OneWayBlock::IsGoingTheOneWay(const Vector2D& v) const {
    // Make sure the given direction is going in relatively the same way as the one
    // way direction for this block...
    return Vector2D::Dot(this->oneWayDir, v) > EPSILON;
}

#endif // __ONEWAYBLOCK_H__