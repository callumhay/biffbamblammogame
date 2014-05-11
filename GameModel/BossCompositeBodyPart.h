/**
 * BossCompositeBodyPart.h
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

#ifndef __COMPOSITEBOSSBODYPART_H__
#define __COMPOSITEBOSSBODYPART_H__

#include "AbstractBossBodyPart.h"

class BossBodyPart;

/**
 * The composite/decorator class of the boss body part hierarchy. Holds composites of
 * body parts (parts containing other parts) for the game's bosses.
 */
class BossCompositeBodyPart : public AbstractBossBodyPart {
public:
    BossCompositeBodyPart();
    ~BossCompositeBodyPart();

    void SetBodyParts(const std::vector<AbstractBossBodyPart*>& parts);
    void AddBodyPart(AbstractBossBodyPart* part);
    void RemoveBodyPart(AbstractBossBodyPart* part); // Not recursive

    // Inherited from AbstractBossBodyPart
    AbstractBossBodyPart::Type GetType() const { return AbstractBossBodyPart::CompositeBodyPart; }
    void Tick(double dT);
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

    void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;
	void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    
    //void TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses);

    bool IsOrContainsPart(const AbstractBossBodyPart* part, bool recursiveSearch) const;
    AbstractBossBodyPart* SearchForParent(AbstractBossBodyPart* part);

    bool GetIsDestroyed() const;
    void SetDestroyed(bool isDestroyed);

    Collision::AABB2D GenerateWorldAABB() const;
    Collision::Circle2D GenerateWorldCircleBounds() const;

    void AnimateColourRGBA(const AnimationMultiLerp<ColourRGBA>& rgbaAnim);
    void ResetColourRGBAAnimation();
    void SetCollisionVelocity(const Vector2D& v);
    void SetExternalAnimationVelocity(const Vector2D& v);
    void SetCollisionAcceleration(const Vector2D& a);

    void DetachProjectile(Projectile* projectile);

#ifdef _DEBUG
    void DebugDraw() const;
#endif

private:
    std::vector<AbstractBossBodyPart*> childParts;
    bool isDestroyed;

    DISALLOW_COPY_AND_ASSIGN(BossCompositeBodyPart);
};


inline void BossCompositeBodyPart::SetBodyParts(const std::vector<AbstractBossBodyPart*>& parts) {
    for (int i = 0; i < static_cast<int>(this->childParts.size()); i++) {
        delete this->childParts[i];
    }
    this->childParts = parts;
}

inline void BossCompositeBodyPart::AddBodyPart(AbstractBossBodyPart* part) {
    this->childParts.push_back(part);
}

#endif // __COMPOSITEBOSSBODYPART_H__