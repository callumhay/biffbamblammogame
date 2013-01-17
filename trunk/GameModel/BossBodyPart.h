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

#include "AbstractBossBodyPart.h"
#include "BoundingLines.h"
#include "GameBall.h"
#include "PlayerPaddle.h"

class Projectile;
class BeamSegment;
class GameModel;

class BossBodyPart : public AbstractBossBodyPart {
public:
    BossBodyPart(const BoundingLines& localBounds);
    virtual ~BossBodyPart();

    const BoundingLines& GetLocalBounds() const;
    BoundingLines GetWorldBounds() const;

	BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
    BossBodyPart* CollisionCheck(const PlayerPaddle& paddle);
	BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT);
	BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir);
	BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir);

    void Translate(const Vector3D& t);
    void RotateZ(float rotZDegs);
    void Transform(const Matrix4x4& m);

    void SetLocalTranslation(const Vector3D& t);

	virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball);
	virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);

    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	virtual void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    
    //virtual void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

    bool IsOrContainsPart(AbstractBossBodyPart* part, bool recursiveSearch) const;

    virtual bool GetIsDestroyed() const;

    Collision::AABB2D GenerateLocalAABB() const;
    Collision::AABB2D GenerateWorldAABB() const;

	// Track the status of the body part, effects its properties and how it works/acts
	//enum PieceStatus { NormalStatus = 0x00000000, OnFireStatus = 0x00000001, FrozenStatus = 0x00000002 };
	//bool HasStatus(const PieceStatus& status) const;
	//void AddStatus(const PieceStatus& status);
	//void RemoveStatus(const PieceStatus& status);
	//void RemoveStatuses(int32_t statusMask);

#ifdef _DEBUG
    void DebugDraw() const;
#endif

protected:
    BoundingLines localBounds;

    //int32_t pieceStatus;

    //void RemoveAllStatus();
    void GetFrozenReflectionRefractionRays(const Point2D& impactPt, const Vector2D& currDir, 
        std::list<Collision::Ray2D>& rays) const;

private:
    DISALLOW_COPY_AND_ASSIGN(BossBodyPart);
};

inline const BoundingLines& BossBodyPart::GetLocalBounds() const {
    return this->localBounds;
}

inline BoundingLines BossBodyPart::GetWorldBounds() const {
    BoundingLines worldBounds(localBounds);
    worldBounds.Transform(this->worldTransform);
    return worldBounds;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                                  Collision::LineSeg2D& collisionLine, double& timeSinceCollision) {
	if (ball.IsLastThingCollidedWith(this)) {
		return false;
	}

    if (this->GetWorldBounds().Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const PlayerPaddle& paddle) {

    if (this->GetWorldBounds().CollisionCheck(paddle.GetBounds().GenerateAABBFromLines())) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const Collision::Ray2D& ray, float& rayT) {
    if (this->GetWorldBounds().CollisionCheck(ray, rayT)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) {
    UNUSED_PARAMETER(velDir);
    if (this->GetWorldBounds().CollisionCheck(boundingLines)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) {
    UNUSED_PARAMETER(velDir);
    if (this->GetWorldBounds().CollisionCheck(c)) {
        return this;
    }
    return NULL;
}

inline void BossBodyPart::Translate(const Vector3D& t) {
    this->worldTransform = Matrix4x4::translationMatrix(t) * this->worldTransform;
}

inline void BossBodyPart::RotateZ(float rotZDegs) {
    if (fabs(rotZDegs) < EPSILON) {
        return;
    }

    Matrix4x4 rotMatrix = Matrix4x4::rotationZMatrix(rotZDegs);
    this->worldTransform = rotMatrix * this->worldTransform;
}

inline void BossBodyPart::Transform(const Matrix4x4& m) {
    this->worldTransform = m * this->worldTransform;
}

inline void BossBodyPart::SetLocalTranslation(const Vector3D& t) {
    // Remove the previous local translation from the world transform
    this->worldTransform = Matrix4x4::translationMatrix(-this->localTranslation) * this->worldTransform;
    
    // Change the local translation and apply it to the world transform
    this->localTranslation = t;
    this->worldTransform = Matrix4x4::translationMatrix(this->localTranslation) * this->worldTransform;
}

inline bool BossBodyPart::GetIsDestroyed() const {
    return false;
}

inline Collision::AABB2D BossBodyPart::GenerateLocalAABB() const {
    return this->GetLocalBounds().GenerateAABBFromLines();
}

inline Collision::AABB2D BossBodyPart::GenerateWorldAABB() const {
    return this->GetWorldBounds().GenerateAABBFromLines();
}

#endif // __BOSSBODYPART_H__