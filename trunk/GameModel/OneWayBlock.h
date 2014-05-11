/**
 * OneWayBlock.h
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

    bool IsExplosionStoppedByPiece(const Point2D& explosionCenter);

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

	// One-way blocks can NEVER be destroyed...
	bool MustBeDestoryedToEndLevel() const {
		return false;
	}
	bool CanBeDestroyedByBall() const {
		return false;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
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

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);

    bool SecondaryCollisionCheck(double dT, const GameBall& ball) const;
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& pointOfCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const;
	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

	void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);
	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

    bool IsGoingTheOneWay(const Vector2D& v) const;

private:
    static const int POINTS_ON_BLOCK_DESTROYED = 700;
    static const float ACCEPTIBLE_MAX_ANGLE_TO_ONE_WAY_IN_RADS;

    OneWayDir dirType;
    Vector2D oneWayDir;

    DISALLOW_COPY_AND_ASSIGN(OneWayBlock);
};

inline bool OneWayBlock::IsExplosionStoppedByPiece(const Point2D& explosionCenter) {
    return !this->IsGoingTheOneWay(this->GetCenter() - explosionCenter);
}

// Whether or not the ball can just blast right through this block.
// Returns: true if it can, false otherwise.
inline bool OneWayBlock::BallBlastsThrough(const GameBall& b) const {
    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return this->IsGoingTheOneWay(b.GetDirection());
}

inline bool OneWayBlock::SecondaryCollisionCheck(double dT, const GameBall& ball) const {
    UNUSED_PARAMETER(dT);

    // Collisions only occur if the ball is not going 'the one way' or if the piece is frozen in ice
    if (this->IsGoingTheOneWay(ball.GetDirection()) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return true;
}

inline bool OneWayBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                        Collision::LineSeg2D& collisionLine,
                                        double& timeUntilCollision, Point2D& pointOfCollision) const {

    if (!this->SecondaryCollisionCheck(dT, ball)) {
        return false;
    }

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), 
                                n, collisionLine, timeUntilCollision, pointOfCollision);
}

inline bool OneWayBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    if (this->IsGoingTheOneWay(ray.GetUnitDirection()) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return Collision::IsCollision(ray, this->GetAABB(), rayT);
}

inline bool OneWayBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
    if (this->IsGoingTheOneWay(velocity) && !this->HasStatus(LevelPiece::IceCubeStatus)) {
        return false;
    }
    return this->bounds.CollisionCheck(boundingLines, dT, velocity);
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
    float dot = Vector2D::Dot(this->oneWayDir, v);
    if (dot >= 0.0) {
        float angleToOneWayInRads = acos(std::min<float>(1.0f, dot));
        return (angleToOneWayInRads <= ACCEPTIBLE_MAX_ANGLE_TO_ONE_WAY_IN_RADS);
    }
    
    return false;
}

#endif // __ONEWAYBLOCK_H__