/**
 * BossBodyPart.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSBODYPART_H__
#define __BOSSBODYPART_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Collision.h"

#include "BoundingLines.h"
#include "GameBall.h"
#include "PlayerPaddle.h"

class Projectile;
class BeamSegment;
class GameModel;

class BossBodyPart {
public:
    BossBodyPart(const BoundingLines& bounds);
    virtual ~BossBodyPart();

    void Tick(double dT);

	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	bool CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

    void SetMovementAnimation(const AnimationMultiLerp<float>& velocityAnim,
        const AnimationMultiLerp<Vector2D>& movementAnim);

	virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball);
	virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);

    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	virtual void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    
    //virtual void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

    virtual bool GetIsDestroyed() const;

	// Track the status of the body part, effects its properties and how it works/acts
	//enum PieceStatus { NormalStatus = 0x00000000, OnFireStatus = 0x00000001, FrozenStatus = 0x00000002 };
	//bool HasStatus(const PieceStatus& status) const;
	//void AddStatus(const PieceStatus& status);
	//void RemoveStatus(const PieceStatus& status);
	//void RemoveStatuses(int32_t statusMask);

protected:

    AnimationMultiLerp<float> velocityMagAnim;
    AnimationMultiLerp<Vector2D> movementDirAnim;

    BoundingLines bounds;

    //int32_t pieceStatus;

    //void RemoveAllStatus();
    void GetFrozenReflectionRefractionRays(const Point2D& impactPt, const Vector2D& currDir, 
        std::list<Collision::Ray2D>& rays) const;

private:
    DISALLOW_COPY_AND_ASSIGN(BossBodyPart);
};

inline bool BossBodyPart::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                         Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	if (ball.IsLastThingCollidedWith(this)) {
		return false;
	}

	return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
}

inline bool BossBodyPart::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    return this->bounds.CollisionCheck(ray, rayT);
}

inline bool BossBodyPart::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
    return this->bounds.CollisionCheck(boundingLines);
}

inline bool BossBodyPart::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    UNUSED_PARAMETER(velDir);
	return this->bounds.CollisionCheck(c);
}

inline void BossBodyPart::SetMovementAnimation(const AnimationMultiLerp<float>& velocityAnim, 
                                               const AnimationMultiLerp<Vector2D>& movementAnim) {
    this->velocityMagAnim = velocityAnim;
    this->movementDirAnim = movementAnim;
}

inline bool BossBodyPart::GetIsDestroyed() const {
    return false;
}

#endif // __BOSSBODYPART_H__