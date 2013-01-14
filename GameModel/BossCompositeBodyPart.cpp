/**
 * BossCompositeBodyPart.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossCompositeBodyPart.h"
#include "BossBodyPart.h"

BossCompositeBodyPart::BossCompositeBodyPart() : AbstractBossBodyPart() {
}

BossCompositeBodyPart::~BossCompositeBodyPart() {
}

BossBodyPart* BossCompositeBodyPart::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                                    Collision::LineSeg2D& collisionLine, double& timeSinceCollision) {

    double bestTimeSinceCollision = DBL_MAX;
    BossBodyPart* bestChoice = NULL;
    Vector2D currNormal(0,0);
    Collision::LineSeg2D currLineSeg;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(ball, dT, currNormal, currLineSeg, timeSinceCollision);
        
        if (result != NULL) {
            if (bestTimeSinceCollision > timeSinceCollision) {
                
                bestTimeSinceCollision = timeSinceCollision;
                n = currNormal;
                collisionLine = currLineSeg;
                bestChoice = result;
            }
        }
    }

    timeSinceCollision = bestTimeSinceCollision;
    return bestChoice;
}

void BossCompositeBodyPart::Tick(double dT, GameModel* gameModel) {

    // Move the body part by whatever velocity is currently set by its movement animation
    this->velocityMagAnim.Tick(dT);
    this->movementDirAnim.Tick(dT);
    Vector3D dMovement = (dT * this->velocityMagAnim.GetInterpolantValue()) * this->movementDirAnim.GetInterpolantValue();
    this->Translate(dMovement);

    // Tick all of the composite parts
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Tick(dT, gameModel);
    }
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
                                                    const Vector2D& velDir) {

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        BossBodyPart* result = part->CollisionCheck(boundingLines, velDir);
        
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

void BossCompositeBodyPart::RotateZ(float rotZDegs) {
    if (fabs(rotZDegs) < EPSILON) {
        return;
    }

    Matrix4x4 rotMatrix = Matrix4x4::rotationZMatrix(rotZDegs);
    this->worldTransform = rotMatrix * this->worldTransform;

    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(rotMatrix);
    }
}

void BossCompositeBodyPart::Transform(const Matrix4x4& m) {
    this->worldTransform = m * this->worldTransform;
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        AbstractBossBodyPart* part = this->childParts[i];
        part->Transform(m);
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

Collision::AABB2D BossCompositeBodyPart::GenerateLocalAABB() const {
    int i = 0;
    Collision::AABB2D result;
    while (result.IsEmpty() && i < static_cast<int>(this->childParts.size())) {
        const AbstractBossBodyPart* part = this->childParts[i];
        result = part->GenerateLocalAABB();
        i++;
    }

    Collision::AABB2D temp;
    for (; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        temp = part->GenerateLocalAABB();
        if (!temp.IsEmpty()) {
            result.AddAABB(temp);
        }
    }

    return result;
}

Collision::AABB2D BossCompositeBodyPart::GenerateWorldAABB() const {
    int i = 0;
    Collision::AABB2D result;
    while (result.IsEmpty() && i < static_cast<int>(this->childParts.size())) {
        const AbstractBossBodyPart* part = this->childParts[i];
        result = part->GenerateWorldAABB();
        i++;
    }

    Collision::AABB2D temp;
    for (; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        temp = part->GenerateWorldAABB();
        if (!temp.IsEmpty()) {
            result.AddAABB(temp);
        }
    }

    return result;
}

#ifdef _DEBUG
void BossCompositeBodyPart::DebugDraw() const {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        const AbstractBossBodyPart* part = this->childParts[i];
        part->DebugDraw();
    }
}
#endif