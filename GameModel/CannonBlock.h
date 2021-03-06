/**
 * CannonBlock.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    
    static const double BALL_CAMERA_ROTATION_TIME_IN_SECS;

	CannonBlock(unsigned int wLoc, unsigned int hLoc, int setRotation);
    CannonBlock(unsigned int wLoc, unsigned int hLoc, const std::pair<int, int>& rotationInterval);
	virtual ~CannonBlock();

	virtual LevelPieceType GetType() const { 
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
	bool BallBlastsThrough(const GameBall&) const {
		return false;
	}
	bool GhostballPassesThrough() const {
		return false;
	}
	bool IsLightReflectorRefractor() const {
		return false;
	}

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const {
        UNUSED_PARAMETER(changeToPiece);
        return 0; 
    }

	// The regular cannon block cannot be destroyed
	virtual LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	
    bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, 
        double& timeUntilCollision, Point2D& pointOfCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;
	
    void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
										const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
										const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
										const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

    void DrawWireframe() const;
    static void DrawCannonDirIntoWireframe(const Point2D& center, const Vector2D& cannonDir);

    void SetRotationSpeed(int dir, float magnitudePercent);
    void Fire();
    void InitBallCameraInCannonValues(bool changeRotation, const GameBall& ball);

    bool TestRotateAndFire(double dT, bool overrideFireRotation, Vector2D& firingDir) const;
    std::pair<LevelPiece*, bool> RotateAndFire(double dT, GameModel* gameModel, bool overrideFireRotation);

    static Vector2D GetDirectionFromRotationInDegs(float rotFromXInDegs);
	Vector2D GetCurrentCannonDirection() const;
	float GetCurrentCannonAngleInDegs() const;
	Point2D GetEndOfBarrelPoint() const;
	const GameBall* GetLoadedBall() const;
	bool GetIsLoaded() const;

	bool GetHasRandomRotation() const;
	float GetFixedRotationDegsFromX() const;
    float GetRotationSpeed() const;

    double GetElapsedRotationTime() const;
    double GetTotalRotationTime() const;

    bool IsProjectileLoaded(const Projectile* p) const { return p == this->loadedProjectile; }

protected:
    virtual LevelPiece* CannonWasFired(GameModel*) { return this; }

private:
	static const double MIN_ROTATION_TIME_IN_SECS;
	static const double MAX_ROTATION_TIME_IN_SECS;

	static const float MIN_ROTATION_SPD_IN_DEGS_PER_SEC;
	static const float MAX_ROTATION_SPD_IN_DEGS_PER_SEC;
    static const float BALL_CAMERA_ROTATION_SPD_IN_DEGS_PER_SEC;

	static const float MIN_DEGREES_PER_FIXED_ROTATION;
	static const float MAX_DEGREES_PER_FIXED_ROTATION;

	GameBall* loadedBall;
    Projectile* loadedProjectile;

	float currRotationFromXInDegs;	// The current rotation from the x-axis (default position) in degrees
    float fixedRotationXInDegs;
	float currRotationSpeed;		// The current rotation speed of the cannon
	double elapsedRotationTime;		// The elapsed rotation time from the start of the ball coming into the cannon
	double totalRotationTime;		// The total time before the ball is fired from the cannon
    
    std::pair<int,int> rotationInterval; // The interval of rotation (i.e., the degree angle(s) where the cannon block always fires,
                                         // measured from 12 o'clock), -1 for random

	void SetupRandomCannonFireTimeAndDirection();

	BoundingLines BuildBounds() const;
};

// We need to override this in order to make sure it actually checks for a collision
inline bool CannonBlock::SecondaryCollisionCheck(double dT, const GameBall& ball) const {
    UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(ball);
    return true;
}

inline bool CannonBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, 
										Collision::LineSeg2D& collisionLine, double& timeUntilCollision, 
                                        Point2D& pointOfCollision) const {

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeUntilCollision, pointOfCollision);
}

inline bool CannonBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

inline bool CannonBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
	return this->bounds.CollisionCheck(boundingLines, dT, velocity);
}

inline LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(paddle);
    return this;
}

inline void CannonBlock::SetRotationSpeed(int dir, float magnitudePercent) {
    this->currRotationSpeed    = dir * magnitudePercent * BALL_CAMERA_ROTATION_SPD_IN_DEGS_PER_SEC;
    this->fixedRotationXInDegs = this->currRotationFromXInDegs;
}

// NOTE: This will not immediately fire this cannon block, it will set it up so that the next time
// "RotateAndFire" is called, it will definitely fire.
inline void CannonBlock::Fire() {
    this->elapsedRotationTime  = this->totalRotationTime;
    this->fixedRotationXInDegs = this->currRotationFromXInDegs;
}

inline Vector2D CannonBlock::GetDirectionFromRotationInDegs(float rotFromXInDegs) {
    Vector2D dirVec(1, 0);
    dirVec.Rotate(rotFromXInDegs);
    return dirVec;
}

/**
 * Query the current pointing direction of the cannon.
 * Returns: The normalized direction the cannon is currently pointing in.
 */
inline Vector2D CannonBlock::GetCurrentCannonDirection() const {
    return CannonBlock::GetDirectionFromRotationInDegs(this->currRotationFromXInDegs);
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

inline double CannonBlock::GetElapsedRotationTime() const {
    return this->elapsedRotationTime;
}

inline double CannonBlock::GetTotalRotationTime() const {
    return this->totalRotationTime;
}

#endif // __CANNONBLOCK_H__