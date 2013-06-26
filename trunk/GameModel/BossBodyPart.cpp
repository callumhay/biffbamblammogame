/**
 * BossBodyPart.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BossBodyPart.h"
#include "MineProjectile.h"

BossBodyPart::BossBodyPart(const BoundingLines& localBounds) :
AbstractBossBodyPart(), localBounds(localBounds), isDestroyed(false),
collisionVelocity(0,0), externalAnimationVelocity(0,0) {

    this->rgbaAnim.SetInterpolantValue(ColourRGBA(1,1,1,1));
    this->rgbaAnim.SetRepeat(false);
}

BossBodyPart::~BossBodyPart() {
    this->RemoveAllAttachedProjectiles();
}

void BossBodyPart::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	/*
    // TODO:

    // If this body part is frozen then there are reflection/refraction rays...
    if (this->HasStatus(BossBodyPart::FrozenStatus)) {
		this->GetFrozenReflectionRefractionRays(hitPoint, impactDir, rays);
	}
	else {
		// The default behaviour is to just not do any reflection/refraction and return an empty list
		rays.clear();
	}
    */
    rays.clear();
}

void BossBodyPart::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	UNUSED_PARAMETER(dT);
    UNUSED_PARAMETER(gameModel);
    UNUSED_PARAMETER(beamSegment);

	assert(beamSegment != NULL);
	assert(gameModel != NULL);
}

bool BossBodyPart::IsOrContainsPart(const AbstractBossBodyPart* part, bool recursiveSearch) const {
    if (this == part) {
        return true;
    }
    return false;
}

//void BossBodyPart::TickStatus(double dT, GameModel* gameModel, int32_t& removedStatuses) {  
//}


void BossBodyPart::RemoveAllAttachedProjectiles() {

    // Make sure we remove all attached projectiles as well
    // NOTE: DO NOT USE ITERATORS HERE SINCE THE MINE PROJECTILE DETACHES ITSELF IN THE SetAsFalling call
    while (!this->attachedProjectiles.empty()) {

        Projectile* p = this->attachedProjectiles.begin()->first;
        p->SetLastThingCollidedWith(NULL);

        if (p->IsMine()) {
            assert(dynamic_cast<MineProjectile*>(p) != NULL);
            MineProjectile* mine = static_cast<MineProjectile*>(p);
            mine->DestroyWithoutExplosion();
        }
        this->DetachProjectile(p); // This is redundant and will be ignored, just here for robustness
    }
    this->attachedProjectiles.clear();
}

void BossBodyPart::GetFrozenReflectionRefractionRays(const Point2D& impactPt, const Vector2D& currDir,
                                                     std::list<Collision::Ray2D>& rays) const {

    // TODO.. do this based on the bounds of the body part...
    
}

void BossBodyPart::OnTransformUpdate() {

    // Transform all attached projectiles so that they move with this body part
    for (std::map<Projectile*, Point2D>::iterator iter = this->attachedProjectiles.begin();
         iter != this->attachedProjectiles.end(); ++iter) {

        Projectile* projectile = iter->first;
        const Point2D& projectileLocalPos = iter->second;
        projectile->SetPosition(this->worldTransform * projectileLocalPos);
    }
}

#ifdef _DEBUG
void BossBodyPart::DebugDraw() const {
    this->GetWorldBounds().DebugDraw();
}
#endif