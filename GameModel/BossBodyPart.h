/**
 * BossBodyPart.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

    void ToggleSimpleBoundingCalc(bool on);
    bool GetIsSimpleBoundingCalcOn() const;
    void SetLocalBounds(const BoundingLines& bounds);
    const BoundingLines& GetLocalBounds() const;
    const BoundingLines& GetWorldBounds() const;

    virtual AbstractBossBodyPart::Type GetType() const { return AbstractBossBodyPart::BasicBodyPart; }
    virtual void Tick(double dT);

	BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& pointOfCollision);
    BossBodyPart* CollisionCheck(const PlayerPaddle& paddle);
	BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT);
	BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, double dT, const Vector2D& velocity);
	BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir);

    void Translate(const Vector3D& t);
    void RotateY(float rotYDegs);
    void RotateZ(float rotZDegs);
    void Transform(const Matrix4x4& m);

    void SetLocalTranslation(const Vector3D& t);
    void SetLocalZRotation(float zRotInDegs);
    void SetLocalYRotation(float yRotInDegs);
    void SetLocalTransform(const Vector3D& translation, float zRotInDegs);

    virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball) { UNUSED_PARAMETER(gameModel); UNUSED_PARAMETER(ball); }
    virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile) { UNUSED_PARAMETER(gameModel); UNUSED_PARAMETER(projectile); }
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) { UNUSED_PARAMETER(gameModel); UNUSED_PARAMETER(paddle); }

    virtual bool IsReflectiveRefractive() const { return false; }
    virtual void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	virtual void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    
    //virtual void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

    bool IsOrContainsPart(const AbstractBossBodyPart* part, bool recursiveSearch) const;

    bool GetIsDestroyed() const;
    virtual void SetDestroyed(bool isDestroyed);

    const ColourRGBA& GetColour() const { return this->rgbaAnim.GetInterpolantValue(); }
    float GetAlpha() const { return this->rgbaAnim.GetInterpolantValue().A(); }
    void SetColour(double animateTimeInSecs, const ColourRGBA& colour);
    void AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim);
    void ResetColourRGBAAnimation();

    Collision::AABB2D GenerateWorldAABB() const;

    virtual void ColourAnimationFinished() {};

    void SetCollisionsDisabled(bool disable) { this->collisionsDisabled = disable; }

    void SetCollisionVelocity(const Vector2D& v);
    void SetExternalAnimationVelocity(const Vector2D& v);

    Vector2D GetCollisionVelocity() const;

    void AttachProjectile(Projectile* projectile); 
    void DetachProjectile(Projectile* projectile);

    const std::map<Projectile*, Point2D>& GetAttachedProjectilesMap() const;

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
    bool isDestroyed;
    bool collisionsDisabled;
    bool isSimpleBoundingCalcOn;
    BoundingLines localBounds;
    mutable BoundingLines worldBounds;
    mutable bool isWorldBoundsDirty;

    AnimationMultiLerp<ColourRGBA> rgbaAnim;

    Vector2D collisionVelocity;
    Vector2D externalAnimationVelocity;

    std::map<Projectile*, Point2D> attachedProjectiles; // The attached projectiles, mapped to their local position

    //int32_t pieceStatus;

    void RemoveAllAttachedProjectiles();
    void OnTransformUpdate();

    //void RemoveAllStatus();
    void GetFrozenReflectionRefractionRays(const Point2D& impactPt, const Vector2D& currDir, 
        std::list<Collision::Ray2D>& rays) const;

private:
    DISALLOW_COPY_AND_ASSIGN(BossBodyPart);
};

inline void BossBodyPart::ToggleSimpleBoundingCalc(bool on) {
    this->isSimpleBoundingCalcOn = on;
}

inline bool BossBodyPart::GetIsSimpleBoundingCalcOn() const {
    return this->isSimpleBoundingCalcOn;
}

inline void BossBodyPart::SetLocalBounds(const BoundingLines& bounds) {
    this->localBounds = bounds;
    this->isWorldBoundsDirty = true;
}

inline const BoundingLines& BossBodyPart::GetLocalBounds() const {
    return this->localBounds;
}

inline const BoundingLines& BossBodyPart::GetWorldBounds() const {
    if (this->isWorldBoundsDirty) {
        this->worldBounds = localBounds;
        if (this->isSimpleBoundingCalcOn) {
            this->worldBounds.RotateLinesAndNormals(this->localZRotation, Point2D(0,0));
            this->worldBounds.TranslateBounds(this->worldTransform.getTranslationVec2D());
        }
        else {
            this->worldBounds.Transform(this->worldTransform);
        }
        this->isWorldBoundsDirty = false; 
    }
    return this->worldBounds;
}

inline void BossBodyPart::Tick(double dT) {
    AbstractBossBodyPart::Tick(dT);
    if (this->rgbaAnim.Tick(dT)) {
        this->ColourAnimationFinished();
    }
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                                  Collision::LineSeg2D& collisionLine, 
                                                  double& timeUntilCollision, Point2D& pointOfCollision) {

    if (this->collisionsDisabled) {
        return NULL;
    }

    Vector2D bossVelocity = this->GetCollisionVelocity();

    if (this->GetWorldBounds().Collide(dT, ball.GetBounds(), ball.GetVelocity(), 
        n, collisionLine, timeUntilCollision, pointOfCollision, bossVelocity)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const PlayerPaddle& paddle) {
    if (this->collisionsDisabled) {
        return NULL;
    }
    if (this->GetWorldBounds().CollisionCheck(paddle.GetBounds().GenerateAABBFromLines())) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const Collision::Ray2D& ray, float& rayT) {
    if (this->collisionsDisabled) {
        return NULL;
    }
    if (this->GetWorldBounds().CollisionCheck(ray, rayT)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const BoundingLines& boundingLines, 
                                                  double dT, const Vector2D& velocity) {
    if (this->collisionsDisabled) {
        return NULL;
    }
    if (this->GetWorldBounds().CollisionCheck(boundingLines, dT, velocity)) {
        return this;
    }
    return NULL;
}

inline BossBodyPart* BossBodyPart::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) {
    UNUSED_PARAMETER(velDir);
    if (this->collisionsDisabled) {
        return NULL;
    }
    if (this->GetWorldBounds().CollisionCheck(c)) {
        return this;
    }
    return NULL;
}

inline void BossBodyPart::Translate(const Vector3D& t) {
    this->worldTransform = Matrix4x4::translationMatrix(t) * this->worldTransform;
    this->OnTransformUpdate();
}

inline void BossBodyPart::RotateY(float rotYDegs) {
    this->worldTransform = Matrix4x4::rotationYMatrix(rotYDegs) * this->worldTransform;
    this->OnTransformUpdate();
}

inline void BossBodyPart::RotateZ(float rotZDegs) {
    this->worldTransform = Matrix4x4::rotationZMatrix(rotZDegs) * this->worldTransform;
    this->OnTransformUpdate();
}

inline void BossBodyPart::Transform(const Matrix4x4& m) {
    this->worldTransform = m * this->worldTransform;
    this->OnTransformUpdate();
}

inline void BossBodyPart::SetLocalTranslation(const Vector3D& t) {
    // Remove the previous local rotation and translation from the world transform,
    // change the translation and then rebuild it
    this->worldTransform = 
        this->worldTransform *
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) *
        Matrix4x4::translationMatrix(-this->localTranslation + t) *
        Matrix4x4::rotationYMatrix(this->localYRotation) *
        Matrix4x4::rotationZMatrix(this->localZRotation);
    
    // Change the local translation
    this->localTranslation = t;

    this->OnTransformUpdate();
}

inline void BossBodyPart::SetLocalZRotation(float zRotInDegs) {
    // Remove the previous rotation...
    this->worldTransform =
        this->worldTransform *
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationZMatrix(zRotInDegs);

    // Change the local z-axis rotation
    this->localZRotation = zRotInDegs;

    this->OnTransformUpdate();
}

inline void BossBodyPart::SetLocalYRotation(float yRotInDegs) {
    // Remove the previous rotation...
    this->worldTransform =
        this->worldTransform *
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) *
        Matrix4x4::rotationYMatrix(yRotInDegs) *
        Matrix4x4::rotationZMatrix(this->localZRotation);

    // Change the local y-axis rotation
    this->localYRotation = yRotInDegs;

    this->OnTransformUpdate();
}

inline void BossBodyPart::SetLocalTransform(const Vector3D& translation, float zRotInDegs) {
    // Remove the previous local rotation and translation from the world transform
    this->worldTransform = 
        this->worldTransform *
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) *
        Matrix4x4::translationMatrix(-this->localTranslation + translation) *
        Matrix4x4::rotationYMatrix(this->localYRotation) *
        Matrix4x4::rotationZMatrix(zRotInDegs);
    
    // Change the local translation and z-axis rotation
    this->localTranslation = translation;
    this->localZRotation   = zRotInDegs;

    this->OnTransformUpdate();
}

inline void BossBodyPart::SetColour(double animateTimeInSecs, const ColourRGBA& colour) {
    this->rgbaAnim.SetLerp(animateTimeInSecs, colour);
}

inline void BossBodyPart::AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim) {
    this->rgbaAnim = rgbaAnim;
}

inline void BossBodyPart::ResetColourRGBAAnimation() {
    this->rgbaAnim.ClearLerp();
    this->rgbaAnim.SetInterpolantValue(ColourRGBA(1,1,1,1));
    this->rgbaAnim.SetRepeat(false);
    this->ColourAnimationFinished();
}

inline bool BossBodyPart::GetIsDestroyed() const {
    return this->isDestroyed;
}

inline void BossBodyPart::SetDestroyed(bool isDestroyed) {
    this->isDestroyed = isDestroyed;
    if (isDestroyed) {
        this->RemoveAllAttachedProjectiles();
    }
}

inline Collision::AABB2D BossBodyPart::GenerateWorldAABB() const {
    return this->GetWorldBounds().GenerateAABBFromLines();
}

// Sets the velocity to use when doing collisions with this object - allows the
// game to accommodate the velocity that this body part is moving with
inline void BossBodyPart::SetCollisionVelocity(const Vector2D& v) {
    this->collisionVelocity = v;
}
inline void BossBodyPart::SetExternalAnimationVelocity(const Vector2D& v) {
    this->externalAnimationVelocity = v;
}
inline Vector2D BossBodyPart::GetCollisionVelocity() const {
    Vector3D translationAnimVec = this->transAnim.GetDxDt();
    return this->collisionVelocity + Vector2D(translationAnimVec[0], translationAnimVec[1]) + this->externalAnimationVelocity;
}

inline void BossBodyPart::AttachProjectile(Projectile* projectile) {
    assert(projectile != NULL);

    // Figure out the local space position of the projectile by multiplying its world space position
    // by the inverse world transform for this body part
    Point2D projectileLocalPos = this->worldTransform.inverse() * projectile->GetPosition();
    this->attachedProjectiles.insert(std::make_pair(projectile, projectileLocalPos));
}
inline void BossBodyPart::DetachProjectile(Projectile* projectile) {
    assert(projectile != NULL);
    this->attachedProjectiles.erase(projectile);
}

inline const std::map<Projectile*, Point2D>& BossBodyPart::GetAttachedProjectilesMap() const {
    return this->attachedProjectiles;
}

#endif // __BOSSBODYPART_H__