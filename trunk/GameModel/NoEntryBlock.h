/**
 * NoEntryBlock.h
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
    bool CanChangeSelfOrOtherPiecesWhenHit() const { return false; }
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
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, 
        double& timeUntilCollision, Point2D& pointOfCollision) const;
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

    void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);
    void RemoveStatus(GameLevel* level, const PieceStatus& status);
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
                                        double& timeUntilCollision, Point2D& pointOfCollision) const {

    return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), 
                                n, collisionLine, timeUntilCollision, pointOfCollision);
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

inline bool NoEntryBlock::CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity) const {
	return this->bounds.CollisionCheck(boundingLines, dT, velocity);
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