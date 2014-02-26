/**
 * BossBodyPart.cpp
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

#include "BossBodyPart.h"
#include "MineProjectile.h"

BossBodyPart::BossBodyPart(const BoundingLines& localBounds) :
AbstractBossBodyPart(), localBounds(localBounds), worldBounds(localBounds), 
isDestroyed(false), collisionsDisabled(false), collisionVelocity(0,0), 
externalAnimationVelocity(0,0), isSimpleBoundingCalcOn(false)  {

    this->rgbaAnim.SetInterpolantValue(ColourRGBA(1,1,1,1));
    this->rgbaAnim.SetRepeat(false);
}

BossBodyPart::~BossBodyPart() {
    this->RemoveAllAttachedProjectiles();
}

void BossBodyPart::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, std::list<Collision::Ray2D>& rays) const {
	UNUSED_PARAMETER(hitPoint);
    UNUSED_PARAMETER(impactDir);
    UNUSED_PARAMETER(rays);
    /*
    // TODO?

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
    UNUSED_PARAMETER(recursiveSearch);
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
    UNUSED_PARAMETER(impactPt);
    UNUSED_PARAMETER(currDir);
    UNUSED_PARAMETER(rays);

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

    // World-space boundaries of this part are no longer up-to-date
    this->isWorldBoundsDirty = true;
}

#ifdef _DEBUG
void BossBodyPart::DebugDraw() const {
    this->GetWorldBounds().DebugDraw();
}
#endif