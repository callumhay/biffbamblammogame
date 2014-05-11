/**
 * BossCompositeBodyPart.cpp
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

#include "BossCompositeBodyPart.h"
#include "BossBodyPart.h"

BossCompositeBodyPart::BossCompositeBodyPart() : AbstractBossBodyPart() {
}

BossCompositeBodyPart::~BossCompositeBodyPart() {
}

void BossCompositeBodyPart::RemoveBodyPart(AbstractBossBodyPart* part) {
    std::vector<AbstractBossBodyPart*>::iterator findIter = this->childParts.begin();
    for (; findIter != this->childParts.end(); ++findIter) {
        if (*findIter == part) {
            this->childParts.erase(findIter);
            return;
        }
    }
}

void BossCompositeBodyPart::Tick(double dT) {
    AbstractBossBodyPart::Tick(dT);
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Tick(dT);
    }
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                                    Collision::LineSeg2D& collisionLine, 
                                                    double& timeUntilCollision, Point2D& pointOfCollision) {

    double bestTimeSinceCollision = DBL_MAX;
    BossBodyPart* bestChoice = NULL;
    Vector2D currNormal(0,0);
    Collision::LineSeg2D currLineSeg;
    Point2D currPtOfCollision;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(ball, dT, currNormal, currLineSeg, timeUntilCollision, currPtOfCollision);
        
        if (result != NULL) {
            if (bestTimeSinceCollision > timeUntilCollision) {
                
                bestTimeSinceCollision = timeUntilCollision;
                n = currNormal;
                collisionLine = currLineSeg;
                pointOfCollision = currPtOfCollision;
                bestChoice = result;
            }
        }
    }

    timeUntilCollision = bestTimeSinceCollision;
    return bestChoice;
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const PlayerPaddle& paddle) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(paddle);
        
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const Collision::Ray2D& ray, float& rayT) {
    float bestRayT = FLT_MAX;
    BossBodyPart* bestChoice = NULL;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(ray, rayT);
        
        if (result != NULL) {
            if (bestRayT > rayT) {
                bestRayT = rayT;
                bestChoice = result;
            }
        }
    }

    rayT = bestRayT;
    return bestChoice;
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const BoundingLines& boundingLines,
                                                    double dT, const Vector2D& velocity) {

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(boundingLines, dT, velocity);
        
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) {

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(c, velDir);
        
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

void BossCompositeBodyPart::Translate(const Vector3D& t) {
    Matrix4x4 translationMatrix = Matrix4x4::translationMatrix(t);
    this->worldTransform = translationMatrix * this->worldTransform;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(translationMatrix);
    }
}

void BossCompositeBodyPart::AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->AnimateColourRGBA(rgbaAnim);
    }
}

void BossCompositeBodyPart::ResetColourRGBAAnimation() {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->ResetColourRGBAAnimation();
    }
}

void BossCompositeBodyPart::SetCollisionVelocity(const Vector2D& v) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->SetCollisionVelocity(v);      
    }
}

void BossCompositeBodyPart::SetExternalAnimationVelocity(const Vector2D& v) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->SetExternalAnimationVelocity(v);      
    }
}

void BossCompositeBodyPart::DetachProjectile(Projectile* projectile) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->DetachProjectile(projectile);      
    }
}

void BossCompositeBodyPart::RotateY(float rotYDegs) {
    if (fabs(rotYDegs) < EPSILON) {
        return;
    }

    Matrix4x4 rotMatrix = Matrix4x4::rotationYMatrix(rotYDegs);
    this->Transform(rotMatrix);
}

void BossCompositeBodyPart::RotateZ(float rotZDegs) {
    if (fabs(rotZDegs) < EPSILON) {
        return;
    }

    Matrix4x4 rotMatrix = Matrix4x4::rotationZMatrix(rotZDegs);
    this->Transform(rotMatrix);
}

void BossCompositeBodyPart::Transform(const Matrix4x4& m) {
    this->worldTransform = m * this->worldTransform;
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(m);
    }
}

void BossCompositeBodyPart::SetLocalTranslation(const Vector3D& t) {
    Matrix4x4 changeTransform =
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) * 
        Matrix4x4::translationMatrix(-this->localTranslation + t) *
        Matrix4x4::rotationYMatrix(this->localYRotation) * 
        Matrix4x4::rotationZMatrix(this->localZRotation);

    Matrix4x4 prevWorldTransform = this->worldTransform;
    this->worldTransform = prevWorldTransform * changeTransform;
    changeTransform = this->worldTransform * prevWorldTransform.inverse();

    this->localTranslation = t;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(changeTransform);
    }
}

void BossCompositeBodyPart::SetLocalZRotation(float zRotInDegs) {
    Matrix4x4 changeTransform = 
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationZMatrix(zRotInDegs);

    Matrix4x4 prevWorldTransform = this->worldTransform;
    this->worldTransform = prevWorldTransform * changeTransform;
    changeTransform = this->worldTransform * prevWorldTransform.inverse();

    this->localZRotation = zRotInDegs;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(changeTransform);
    }
}

void BossCompositeBodyPart::SetLocalYRotation(float yRotInDegs) {
    Matrix4x4 changeTransform = 
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) *
        Matrix4x4::rotationYMatrix(yRotInDegs) *
        Matrix4x4::rotationZMatrix(this->localZRotation);

    Matrix4x4 prevWorldTransform = this->worldTransform;
    this->worldTransform = prevWorldTransform * changeTransform;
    changeTransform = this->worldTransform * prevWorldTransform.inverse();

    this->localYRotation = yRotInDegs;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(changeTransform);
    }
}

void BossCompositeBodyPart::SetLocalTransform(const Vector3D& translation, float zRotInDegs) {
    Matrix4x4 changeTransform = 
        Matrix4x4::rotationZMatrix(-this->localZRotation) *
        Matrix4x4::rotationYMatrix(-this->localYRotation) * 
        Matrix4x4::translationMatrix(-this->localTranslation) *
        Matrix4x4::translationMatrix(translation) *
        Matrix4x4::rotationYMatrix(this->localYRotation) * 
        Matrix4x4::rotationZMatrix(zRotInDegs);

    Matrix4x4 prevWorldTransform = this->worldTransform;
    this->worldTransform = prevWorldTransform * changeTransform;
    changeTransform = this->worldTransform * prevWorldTransform.inverse();

    this->localTranslation = translation;
    this->localZRotation   = zRotInDegs;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(changeTransform);
    }
}

void BossCompositeBodyPart::GetReflectionRefractionRays(const Point2D& hitPoint,
                                                        const Vector2D& impactDir,
                                                        std::list<Collision::Ray2D>& rays) const {
    rays.clear();
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->GetReflectionRefractionRays(hitPoint, impactDir, rays);
        if (!rays.empty()) {
            return;
        }
    }
}

void BossCompositeBodyPart::TickBeamCollision(double dT, const BeamSegment* beamSegment,
                                              GameModel* gameModel) {

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->TickBeamCollision(dT, beamSegment, gameModel);
    }
}

bool BossCompositeBodyPart::IsOrContainsPart(const AbstractBossBodyPart* part, bool recursiveSearch) const {
    if (this == part) {
        return true;
    }

    if (recursiveSearch) {

        for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
            AbstractBossBodyPart* currPart = this->childParts[i];
            if (part == currPart || currPart->IsOrContainsPart(part, true)) {
                return true;
            }
        }
    }
    else {
        for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
            AbstractBossBodyPart* currPart = this->childParts[i];
            if (part == currPart) {
                return true;
            }
        }
    }
    return false;
}

AbstractBossBodyPart* BossCompositeBodyPart::SearchForParent(AbstractBossBodyPart* part) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* currPart = this->childParts[i];
        if (part == currPart) {
            return this;
        }
        else {
            AbstractBossBodyPart* result = currPart->SearchForParent(part);
            if (result != NULL) {
                return result;
            }
        }
    }

    return NULL;
}

//void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

bool BossCompositeBodyPart::GetIsDestroyed() const {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        if (!part->GetIsDestroyed()) {
            return false;
        }
    }
    return true;
}

void BossCompositeBodyPart::SetDestroyed(bool isDestroyed) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->SetDestroyed(isDestroyed);
    }
}

Collision::AABB2D BossCompositeBodyPart::GenerateWorldAABB() const {
    
    Collision::AABB2D result;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        result.AddAABB(part->GenerateWorldAABB());
    }

    return result;
}

Collision::Circle2D BossCompositeBodyPart::GenerateWorldCircleBounds() const {
    Collision::Circle2D result;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        result.AddCircle(part->GenerateWorldCircleBounds());
    }

    return result;
}

#ifdef _DEBUG
void BossCompositeBodyPart::DebugDraw() const {
    // Show the AABB for this part
    Collision::AABB2D aabb = this->GenerateWorldAABB();
    aabb.DebugDraw();

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        part->DebugDraw();
    }
}
#endif