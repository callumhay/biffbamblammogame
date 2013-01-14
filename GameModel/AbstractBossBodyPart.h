/**
 * AbstractBossBodyPart.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ABSTRACTBOSSBODYPART_H__
#define __ABSTRACTBOSSBODYPART_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Matrix.h"

class GameBall;
class BoundingLines;
class GameModel;
class Projectile;
class PlayerPaddle;
class BeamSegment;
class BossBodyPart;

class AbstractBossBodyPart {
public:
    
    AbstractBossBodyPart();
    virtual ~AbstractBossBodyPart();

    virtual void Tick(double dT, GameModel* gameModel) = 0;

    virtual BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision) = 0;
	virtual BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT) = 0;
	virtual BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) = 0;
	virtual BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) = 0;

    void SetMovementAnimation(const AnimationMultiLerp<float>& velocityAnim,
        const AnimationMultiLerp<Vector3D>& movementAnim);
    //void SetRotationAnimation(const AnimationMultiLerp<float>& degreesAnim,
    //    const AnimationMultiLerp<Vector3D>& rotationAnim);

    virtual void Translate(const Vector3D& t)  = 0;
    virtual void RotateZ(float rotZDegs)       = 0;
    virtual void Transform(const Matrix4x4& m) = 0;

    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const = 0;
	virtual void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) = 0;
    
    //virtual void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses) = 0;

    virtual bool GetIsDestroyed() const = 0;

    virtual Collision::AABB2D GenerateLocalAABB() const = 0;
    virtual Collision::AABB2D GenerateWorldAABB() const = 0;

    const AnimationMultiLerp<float>& GetVelocityMagAnim() const { return this->velocityMagAnim; }
    const AnimationMultiLerp<Vector3D>& GetMovementDirAnim() const { return this->movementDirAnim; }

    const Matrix4x4& GetWorldTransform() const { return this->worldTransform; }
    Vector2D GetTranslationVec2D() const { return this->worldTransform.getTranslationVec2D(); }
    Point2D GetTranslationPt2D() const { return this->worldTransform.getTranslationPt2D(); }
    Point3D GetTranslationPt3D() const { return this->worldTransform.getTranslationPt3D(); }
    void SetWorldTranslation(const Point3D& t) { this->worldTransform.setTranslation(t); }
    


#ifdef _DEBUG
    virtual void DebugDraw() const = 0;
#endif

protected:
    Matrix4x4 worldTransform;

    AnimationMultiLerp<float> velocityMagAnim;
    AnimationMultiLerp<Vector3D> movementDirAnim;

private:
    DISALLOW_COPY_AND_ASSIGN(AbstractBossBodyPart);
};

inline AbstractBossBodyPart::AbstractBossBodyPart() :
velocityMagAnim(0.0f), movementDirAnim(Vector3D(0.0f, 0.0f, 0.0f)) {

    this->velocityMagAnim.ClearLerp();
    this->velocityMagAnim.SetInterpolantValue(0.0f);
    this->movementDirAnim.ClearLerp();
    this->movementDirAnim.SetInterpolantValue(Vector3D(0.0f, 0.0f, 0.0f));
}

inline AbstractBossBodyPart::~AbstractBossBodyPart() {
}

inline void AbstractBossBodyPart::SetMovementAnimation(const AnimationMultiLerp<float>& velocityAnim, 
                                                       const AnimationMultiLerp<Vector3D>& movementAnim) {
    this->velocityMagAnim = velocityAnim;
    this->movementDirAnim = movementAnim;
}

#endif // __ABSTRACTBOSSBODYPART_H__