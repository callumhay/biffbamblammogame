/**
 * CannonBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CANNONBLOCK_H__
#define __CANNONBLOCK_H__

#include "LevelPiece.h"

class CannonBlock : public LevelPiece {

public:
	static const float CANNON_BARREL_LENGTH;
	static const float HALF_CANNON_BARREL_LENGTH;
	static const float CANNON_BARREL_HEIGHT;
	static const float HALF_CANNON_BARREL_HEIGHT;

	CannonBlock(unsigned int wLoc, unsigned int hLoc);
	~CannonBlock();

	LevelPieceType GetType() const { 
		return LevelPiece::Cannon;
	}

	// Cannon blocks have no bounds per-se... the ball doesn't bounce off of them,
	// instead, much like the portal block, they manipulate ball properties when it hits
	bool IsNoBoundsPieceType() const {
		return true;
	}
	bool BallBouncesOffWhenHit() const {
		return this->hasBall;
	}

	// Cannon blocks don't need to be destroyed to end a level... in fact they cannot be destroyed
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyed() const {
		return false;
	}
	
	// Even the uber ball gets captured by the nefarious net block!
	bool UberballBlastsThrough() const {
		return false;
	}

	// Whether or not the ghost ball can just pass through this block.
	bool GhostballPassesThrough() const {
		return true;
	}

	// Obtain the point value for a collision with this block.
	int GetPointValueForCollision() {
		return 0;
	}

	// Particles just extinguish on contact with a cannon block
	bool ProjectilePassesThrough(Projectile* projectile) {
		return false;
	}

	// Light beams will extinguish on contact
	bool IsLightReflectorRefractor() const {
		return false;
	}

	// The cannon block cannot be destroyed
	LevelPiece* Destroy(GameModel* gameModel) { return this; }
	
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	//bool CollisionCheck(const Collision::AABB2D& aabb) const;
	//bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	//bool CollisionCheck(const BoundingLines& boundingLines) const;
	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile) { return this->Destroy(gameModel); }

	bool RotateAndEventuallyFire(double dT);
	Vector2D GetCurrentCannonDirection() const;
	float GetCurrentCannonAngleInDegs() const;

private:
	static const double MIN_ROTATION_TIME_IN_SECS;
	static const double MAX_ROTATION_TIME_IN_SECS;

	static const float MIN_ROTATION_SPD_IN_DEGS_PER_SEC;
	static const float MAX_ROTATION_SPD_IN_DEGS_PER_SEC;

	bool hasBall;
	float currRotationFromXInDegs;	// The current rotation from the x-axis (default position) in degrees
	float currRotationSpeed;				// The current rotation speed of the cannon
	double elapsedRotationTime;			// The elapsed rotation time from the start of the ball coming into the cannon
	double totalRotationTime;				// The total time before the ball is fired from the cannon
};

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

#endif // __CANNONBLOCK_H__