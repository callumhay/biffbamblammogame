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
#include "../BlammoEngine/Colour.h"

class GameBall;
class BoundingLines;
class GameModel;
class Projectile;
class PlayerPaddle;
class BeamSegment;
class BossBodyPart;

class AbstractBossBodyPart {
public:
    enum Type { BasicBodyPart, WeakpointBodyPart, CompositeBodyPart };

    AbstractBossBodyPart();
    virtual ~AbstractBossBodyPart();

    virtual AbstractBossBodyPart::Type GetType() const = 0;

    virtual void Tick(double dT);

    virtual BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision) = 0;
    virtual BossBodyPart* CollisionCheck(const PlayerPaddle& paddle) = 0;
	virtual BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT) = 0;
	virtual BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) = 0;
	virtual BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) = 0;

    virtual void Translate(const Vector3D& t)  = 0;
    virtual void RotateY(float rotYDegs)       = 0;
    virtual void RotateZ(float rotZDegs)       = 0;
    virtual void Transform(const Matrix4x4& m) = 0;

    virtual void SetLocalTranslation(const Vector3D& t) = 0;
    virtual void SetLocalZRotation(float zRotInDegs)    = 0;
    virtual void SetLocalYRotation(float yRotInDegs)    = 0;
    virtual void SetLocalTransform(const Vector3D& translation, float zRotInDegs) = 0;

    void AnimateLocalTranslation(const AnimationMultiLerp<Vector3D>& animation);
    void ClearLocalTranslationAnimation();
    void AnimateLocalZRotation(const AnimationMultiLerp<float>& animationZDegs);
    void ClearLocalZRotationAnimation();

    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const = 0;
	virtual void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) = 0;
    
    //virtual void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses) = 0;

    virtual bool GetIsDestroyed() const = 0;
    virtual void SetAsDestroyed() = 0;

    virtual Collision::AABB2D GenerateWorldAABB() const = 0;

    const Matrix4x4& GetWorldTransform() const { return this->worldTransform; }

    Vector3D GetTranslationVec3D() const { return this->worldTransform.getTranslationVec3D(); }
    Vector2D GetTranslationVec2D() const { return this->worldTransform.getTranslationVec2D(); }
    Point2D GetTranslationPt2D() const { return this->worldTransform.getTranslationPt2D(); }
    Point3D GetTranslationPt3D() const { return this->worldTransform.getTranslationPt3D(); }

    virtual bool IsOrContainsPart(const AbstractBossBodyPart* part, bool recursiveSearch) const = 0;

    virtual AbstractBossBodyPart* SearchForParent(AbstractBossBodyPart* part) { UNUSED_PARAMETER(part); return NULL; }

    virtual void AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim) = 0;
    virtual void ResetColourRGBAAnimation() = 0;
    virtual void SetCollisionVelocity(const Vector2D& v) = 0;

#ifdef _DEBUG
    virtual void DebugDraw() const = 0;
#endif

protected:
    Matrix4x4 worldTransform;

    Vector3D localTranslation;
    float localZRotation;
    float localYRotation;

    AnimationMultiLerp<Vector3D> transAnim;
    AnimationMultiLerp<float> zRotAnim;

private:
    DISALLOW_COPY_AND_ASSIGN(AbstractBossBodyPart);
};

inline AbstractBossBodyPart::AbstractBossBodyPart() : 
localZRotation(0.0f), localYRotation(0.0f), localTranslation(0.0f, 0.0f, 0.0f) {

    this->ClearLocalTranslationAnimation();
    this->ClearLocalZRotationAnimation();
}

inline AbstractBossBodyPart::~AbstractBossBodyPart() {
}

inline void AbstractBossBodyPart::Tick(double dT) {

    bool isFinishedRot   = this->zRotAnim.Tick(dT);
    bool isFinishedTrans = this->transAnim.Tick(dT);

    if (!isFinishedRot && !isFinishedTrans) {
        float currZRot = this->zRotAnim.GetInterpolantValue();
        const Vector3D& currTranslation = this->transAnim.GetInterpolantValue();
        this->SetLocalTransform(currTranslation, currZRot);
    }
    else {
        if (!isFinishedRot) {
            float currZRot = this->zRotAnim.GetInterpolantValue();
            this->SetLocalZRotation(currZRot);
        }
        if (!isFinishedTrans) {
            const Vector3D& currTranslation = this->transAnim.GetInterpolantValue();
            this->SetLocalTranslation(currTranslation);
        }
    }
}

inline void AbstractBossBodyPart::AnimateLocalTranslation(const AnimationMultiLerp<Vector3D>& animation) {
    this->transAnim = animation;
}

inline void AbstractBossBodyPart::ClearLocalTranslationAnimation() {
    this->transAnim.ClearLerp();
    this->transAnim.SetInterpolantValue(Vector3D(0,0,0));
    this->transAnim.SetRepeat(false);
}

inline void AbstractBossBodyPart::AnimateLocalZRotation(const AnimationMultiLerp<float>& animationZDegs){
    this->zRotAnim = animationZDegs;
}

inline void AbstractBossBodyPart::ClearLocalZRotationAnimation() {
    this->zRotAnim.ClearLerp();
    this->zRotAnim.SetInterpolantValue(0.0f);
    this->zRotAnim.SetRepeat(false);
}

#endif // __ABSTRACTBOSSBODYPART_H__