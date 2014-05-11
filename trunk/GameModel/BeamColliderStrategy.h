/**
 * BeamColliderStrategy.h
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

#ifndef __BEAMCOLLIDERSTRATEGY_H__
#define __BEAMCOLLIDERSTRATEGY_H__

#include "../BlammoEngine/BasicIncludes.h"

class LevelPiece;
class Beam;
class BeamSegment;
class Projectile;
class PlayerPaddle;

class BeamColliderStrategy {
public:
    BeamColliderStrategy(float rayT) : rayT(rayT) {}
    virtual ~BeamColliderStrategy() {}

    float GetRayT() const { return this->rayT; }

    virtual void UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
        std::set<const void*>& thingsCollidedWith, std::list<BeamSegment*>& newBeamSegs) = 0;

    virtual LevelPiece* GetCollidingPiece() const { return NULL; }
    virtual Projectile* GetCollidingProjectile() const { return NULL; }

protected:
    BeamColliderStrategy() {}

private:
    float rayT;

    DISALLOW_COPY_AND_ASSIGN(BeamColliderStrategy);
};

class LevelPieceBeamColliderStrategy : public BeamColliderStrategy {
public:
    LevelPieceBeamColliderStrategy(float rayT, LevelPiece* collider) : BeamColliderStrategy(rayT), colliderPiece(collider) {}
    ~LevelPieceBeamColliderStrategy() {}

    void UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
        std::set<const void*>& thingsCollidedWith, std::list<BeamSegment*>& newBeamSegs);
    LevelPiece* GetCollidingPiece() const { return this->colliderPiece; }

private:
    LevelPiece* colliderPiece;
    DISALLOW_COPY_AND_ASSIGN(LevelPieceBeamColliderStrategy);
};


class PaddleBeamColliderStrategy : public BeamColliderStrategy {
public:
    PaddleBeamColliderStrategy(float rayT, PlayerPaddle* collider) : BeamColliderStrategy(rayT), colliderPaddle(collider) {}
    ~PaddleBeamColliderStrategy() {}

    void UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
        std::set<const void*>& thingsCollidedWith, std::list<BeamSegment*>& newBeamSegs);
private:
    PlayerPaddle* colliderPaddle;
    DISALLOW_COPY_AND_ASSIGN(PaddleBeamColliderStrategy);
};


class ProjectileBeamColliderStrategy : public BeamColliderStrategy {
public:
    ProjectileBeamColliderStrategy(float rayT, Projectile* collider) : BeamColliderStrategy(rayT), colliderProjectile(collider) {}
    ~ProjectileBeamColliderStrategy() {}

    void UpdateBeam(const Beam& beam, BeamSegment* currBeamSegment, 
        std::set<const void*>& thingsCollidedWith, std::list<BeamSegment*>& newBeamSegs);

    Projectile* GetCollidingProjectile() const { return this->colliderProjectile; }

private:
    Projectile* colliderProjectile;
    DISALLOW_COPY_AND_ASSIGN(ProjectileBeamColliderStrategy);
};

#endif // __BEAMCOLLIDERSTRATEGY_H__