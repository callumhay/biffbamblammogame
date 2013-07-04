/**
 * TargetedRefractiveBossBodyPart.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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