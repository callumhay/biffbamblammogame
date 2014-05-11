/**
 * TargetedRefractiveBossBodyPart.h
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

#ifndef __TARGETEDREFRACTIVEBOSSBODYPART_H__
#define __TARGETEDREFRACTIVEBOSSBODYPART_H__

class TargetedRefractiveBossBodyPart : public BossBodyPart {
public:
    TargetedRefractiveBossBodyPart(const BoundingLines& localBounds, const Point2D& target) : BossBodyPart(localBounds), targetPt(target) {}
    ~TargetedRefractiveBossBodyPart() {}

    void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    bool IsReflectiveRefractive() const { return true; }
    void GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const;

private:
    Point2D targetPt; // The target point in local space

    DISALLOW_COPY_AND_ASSIGN(TargetedRefractiveBossBodyPart);
};

inline void TargetedRefractiveBossBodyPart::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
    UNUSED_PARAMETER(gameModel);

    switch (projectile->GetType()) {
        case Projectile::BallLaserBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
            // Generate reflection/refraction rays...
            if (!projectile->IsLastThingCollidedWith(this)) {

                const float PROJECTILE_VELOCITY_MAG    = projectile->GetVelocityMagnitude();
                const Vector2D PROJECTILE_VELOCITY_DIR = projectile->GetVelocityDirection();
                const Point2D IMPACT_POINT = projectile->GetPosition() + projectile->GetHalfHeight()*PROJECTILE_VELOCITY_DIR;

                std::list<Collision::Ray2D> rays;
                this->GetReflectionRefractionRays(IMPACT_POINT, PROJECTILE_VELOCITY_DIR, rays);
                assert(rays.size() == 1);
                
                const Collision::Ray2D& refractionRay = *rays.begin();

                // The one and only ray is how the current projectile gets transmitted through this block...
                projectile->SetPosition(refractionRay.GetOrigin());
                projectile->SetVelocity(refractionRay.GetUnitDirection(), PROJECTILE_VELOCITY_MAG);
                projectile->SetLastThingCollidedWith(this);
            }
            break;
        default:
            break;
    }
}

inline void TargetedRefractiveBossBodyPart::GetReflectionRefractionRays(const Point2D& hitPoint, 
                                                                        const Vector2D& impactDir, 
                                                                        std::list<Collision::Ray2D>& rays) const {
    UNUSED_PARAMETER(impactDir);

    // The hit point will be in world space, we need to bring the target point into world space as well...
    Vector2D refractionDir = (this->worldTransform * this->targetPt) - hitPoint;
    refractionDir.Normalize();

    rays.clear();
    rays.push_back(Collision::Ray2D(hitPoint, refractionDir));
}

#endif // __TARGETEDREFRACTIVEBOSSBODYPART_H__