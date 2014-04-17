/**
 * BeamColliderStrategy.cpp
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

#include "BeamColliderStrategy.h"
#include "Beam.h"
#include "LevelPiece.h"
#include "PortalBlock.h"
#include "PortalProjectile.h"

void LevelPieceBeamColliderStrategy::UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
                                                std::set<const void*>& thingsCollidedWith, 
                                                std::list<BeamSegment*>& newBeamSegs) {

    // Check to see if the piece was already hit by this beam...
    std::pair<std::set<const void*>::iterator, bool> insertResult = thingsCollidedWith.insert(this->colliderPiece);
    if (!insertResult.second) {
        return;
    }

    // This is the first time the level piece was hit by this beam - we are allowed to spawn more beams...
    // The piece may generate a set of spawned beams based on whether or not it reflects/refracts light
    const Collision::Ray2D& currBeamRay = currBeamSegment->GetBeamSegmentRay();
    std::list<Collision::Ray2D> spawnedRays;
    this->colliderPiece->GetReflectionRefractionRays(currBeamSegment->GetEndPoint(), currBeamRay.GetUnitDirection(), spawnedRays);

    LevelPiece* ignorePiece = this->colliderPiece;

    // In the case where a portal block is collided with then we need to account for its sibling
    // as a "thing collided with" for the new beam(s) we spawn
    if (this->colliderPiece->GetType() == LevelPiece::Portal) {
        PortalBlock* portalBlock = static_cast<PortalBlock*>(this->colliderPiece);
        assert(portalBlock != NULL);
        ignorePiece = portalBlock->GetSiblingPortal();
        thingsCollidedWith.insert(ignorePiece);
    }

    if (!spawnedRays.empty()) {
        // The radius of the spawned beams will be a fraction of the current radius based
        // on the number of reflection/refraction rays
        const float NEW_BEAM_SEGMENT_RADIUS = beam.GetBeamAlpha() * std::max<float>(currBeamSegment->GetRadius() / 
            static_cast<float>(spawnedRays.size()), Beam::MIN_BEAM_RADIUS);

        if (NEW_BEAM_SEGMENT_RADIUS > 0.0f) {
            const int NEW_BEAM_DMG_PER_SECOND = beam.GetBeamAlpha() * std::max<int>(Beam::MIN_DMG_PER_SEC, 
                currBeamSegment->GetDamagePerSecond() / spawnedRays.size());

            // Now add the new beams to the list of beams we need to fire into the level and repeat this whole process with
            std::list<BeamSegment*> spawnedBeamSegs;
            for (std::list<Collision::Ray2D>::iterator iter = spawnedRays.begin(); iter != spawnedRays.end(); ++iter) {
                newBeamSegs.push_back(new BeamSegment(*iter, NEW_BEAM_SEGMENT_RADIUS, NEW_BEAM_DMG_PER_SECOND, ignorePiece));
            }
        }
    }
}

void PaddleBeamColliderStrategy::UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
                                            std::set<const void*>& thingsCollidedWith, std::list<BeamSegment*>& newBeamSegs) {
    UNUSED_PARAMETER(newBeamSegs);

    // Check to see if the paddle was already hit by this beam...
    std::pair<std::set<const void*>::iterator, bool> insertResult = thingsCollidedWith.insert(this->colliderPaddle);
    if (!insertResult.second) {
        return;
    }

    // NOTE: I commented out the shortening of the beam -- it looked odd.
    // Set the new beam ending point - This MUST be done before telling the paddle it was hit!!
    //currBeamSegment->SetLength(minRayT);

    // There's a collision with the paddle
    this->colliderPaddle->HitByBeam(beam, *currBeamSegment);

    // No need to spawn any new beams.
}

void ProjectileBeamColliderStrategy::UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
                                                std::set<const void*>& thingsCollidedWith, 
                                                std::list<BeamSegment*>& newBeamSegs) {

    // Check to see if the projectile was already hit by this beam...
    std::pair<std::set<const void*>::iterator, bool> insertResult = thingsCollidedWith.insert(this->colliderProjectile);
    if (!insertResult.second) {
        return;
    }

    // This is the first time the projectile was hit by this beam - we are allowed to spawn more beams...
    // The projectile may generate a set of spawned beams based on whether or not it reflects/refracts light
    const Collision::Ray2D& currBeamRay = currBeamSegment->GetBeamSegmentRay();
    std::list<Collision::Ray2D> spawnedRays;
    this->colliderProjectile->GetReflectionRefractionRays(currBeamSegment->GetEndPoint(), currBeamRay.GetUnitDirection(), spawnedRays);

    const Projectile* ignoreProjectile = this->colliderProjectile;

    // In the case where a portal block is collided with then we need to account for its sibling
    // as a "thing collided with" for the new beam(s) we spawn
    if (this->colliderProjectile->GetType() == Projectile::PortalBlobProjectile) {
        PortalProjectile* portal = static_cast<PortalProjectile*>(this->colliderProjectile);
        assert(portal != NULL);
        ignoreProjectile = portal->GetSiblingPortal();
        thingsCollidedWith.insert(ignoreProjectile);
    }

    if (!spawnedRays.empty()) {
        // The radius of the spawned beams will be a fraction of the current radius based
        // on the number of reflection/refraction rays
        const float NEW_BEAM_SEGMENT_RADIUS = beam.GetBeamAlpha() * std::max<float>(currBeamSegment->GetRadius() / 
            static_cast<float>(spawnedRays.size()), Beam::MIN_BEAM_RADIUS);

        if (NEW_BEAM_SEGMENT_RADIUS > 0.0f) {
            const int NEW_BEAM_DMG_PER_SECOND = beam.GetBeamAlpha() * std::max<int>(Beam::MIN_DMG_PER_SEC, 
                currBeamSegment->GetDamagePerSecond() / spawnedRays.size());

            // Now add the new beams to the list of beams we need to fire into the level and repeat this whole process with
            std::list<BeamSegment*> spawnedBeamSegs;
            for (std::list<Collision::Ray2D>::iterator iter = spawnedRays.begin(); iter != spawnedRays.end(); ++iter) {
                newBeamSegs.push_back(new BeamSegment(*iter, NEW_BEAM_SEGMENT_RADIUS, NEW_BEAM_DMG_PER_SECOND, ignoreProjectile));
            }
        }
    }
}