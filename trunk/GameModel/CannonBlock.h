/**
 * CannonBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CANNONBLOCK_H__
#define __CANNONBLOCK_H__

#include "LevelPiece.h"

class PaddleRocketProjectile;

class CannonBlock : public LevelPiece {

public:
	static const float CANNON_BARREL_LENGTH;
	static const float HALF_CANNON_BARREL_LENGTH;
	static const float CANNON_BARREL_HEIGHT;
	static const float HALF_CANNON_BARREL_HEIGHT;

	static const int RANDOM_SET_ROTATION;

	CannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation);
    CannonBlock(unsigned int wLoc, unsigned int hLoc, const std::pair<int, int>& rotationInterval);
	~CannonBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Cannon;
	}

    bool IsExplosionStoppedByPiece(const Point2D&) {
        return false;
    }

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall&) const {
        return false; // The ball will NEVER bounce off a cannon
    }

	// Cannon blocks have no bounds per-se... the ball doesn't bounce off of them,
	// instead, much like the portal block, they manipulate ball properties when it hits
	bool IsNoBoundsPieceType() const {
		return true;
	}
	bool BallBouncesOffWhenHit() const {
		return this->loadedBall != NULL;
	}

	// Cannon blocks don't need to be destroyed to end a level... in fact they cannot be destroyed
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return false;
    }
	
	// Even the uber ball just bounces off like a solid block
	bool BallBlastsThrough(const GameBall& b) const {
        UNUSED_PARAMETER(b);
		return false;
	}

	// Ghost ball may pass through
	bool GhostballPassesThrough() const {
		return true;
	}

	// Light beams will extinguish on contact
	bool IsLightReflectorRefractor() const {
		return false;
	}

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0; 
    }

	// The cannon block cannot be destroyed
	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	
    bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeUntilCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;
	
    void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

	bool RotateAndEventuallyFire(double dT);
	Vector2D GetCurrentCannonDirection() const;
	float GetCurrentCannonAngleInDegs() const;
	Point2D GetEndOfBarrelPoint() const;
	const GameBall* GetLoadedBall() const;
	bool GetIsLoaded() const;

	bool GetHasRandomRotation() const;
	float GetFixedRotationDegsFromX() const;
    float GetRotationSpeed() const;

private:
	static const double MIN_ROTATION_TIME_IN_SECS;
	static const double MAX_ROTATION_TIME_IN_SECS;

	static const float MIN_ROTATION_SPD_IN_DEGS_PER_SEC;
	static const float MAX_ROTATION_SPD_IN_DEGS_PER_SEC;

	static const float MIN_DEGREES_PER_FIXED_ROTATION;
	static const float MAX_DEGREES_PER_FIXED_ROTATION;

	GameBall* loadedBall;
    Projectile* loadedProjectile;

	float currRotationFromXInDegs;	// The current rotation from the x-axis (default position) in degrees
	float currRotationSpeed;		// The current rotation speed of the cannon
	double elapsedRotationTime;		// The elapsed rotation time from the start of the ball coming into the cannon
	double totalRotationTime;		// The total time before the ball is fired from the cannon
    
    std::pair<int,int> rotationInterval; // The interval of rotation (i.e., the degree angle(s) where the cannon block always fires,
                                         // measured from 12 o'clock), -1 for random

	void SetupCannonFireTimeAndDirection();

	BoundingLines BuildBounds() const;
};

// We need to override this in order to make sure it actually checks for a collision
inline bool CannonBlock::SecondaryCollisionCheck(double dT, const GameBall& ball) const {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(ball);
    return true;
}

inline bool CannonBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, 
										Collision::LineSeg2D& collisionLine, double& timeUntilCollision) const {

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeUntilCollision);
}

inline bool CannonBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

inline bool CannonBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
	return this->bounds.CollisionCheck(boundingLines, dT, velocity);
}

/**
 * Query the current pointing direction of the cannon.
 * Returns: The normalized direction the cannon is currently pointing in.
 */
inline Vector2D CannonBlock::GetCurrentCannonDirection() const {
	static const Vector2D X_DIR(1, 0);
	Vector2D dir = Vector2D::Rotate(this->currRotationFromXInDegs, X_DIR);
	return Vector2D::Normalize(dir);
}

inline float CannonBlock::GetCurrentCannonAngleInDegs() const {
	return this->currRotationFromXInDegs;
}

// Obtain the point in game space at the end of the cannon barrel
inline Point2D CannonBlock::GetEndOfBarrelPoint() const {
	return this->GetCenter() + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->GetCurrentCannonDirection();
}

inline const GameBall* CannonBlock::GetLoadedBall() const {
	return this->loadedBall;
}

inline bool CannonBlock::GetIsLoaded() const {
	return (this->loadedBall != NULL) || (this->loadedProjectile != NULL);
}

// Gets whether this cannon block fires in random directions or not
inline bool CannonBlock::GetHasRandomRotation() const {
    return (this->rotationInterval.first != this->rotationInterval.second);
}

// Gets the degrees from the x-axis the rotation direction is...
inline float CannonBlock::GetFixedRotationDegsFromX() const {
	assert(!this->GetHasRandomRotation());
	return 90.0f - this->rotationInterval.first;
}

inline float CannonBlock::GetRotationSpeed() const {
    return this->currRotationSpeed;
}

#endif // __CANNONBLOCK_H__