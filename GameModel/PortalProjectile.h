/**
 * PortalProjectile.h
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

#ifndef __PORTALPROJECTILE_H__
#define __PORTALPROJECTILE_H__

#include "Projectile.h"

class PortalProjectile : public Projectile {
public:
    static std::pair<PortalProjectile*, PortalProjectile*> BuildSiblingPortalProjectiles(
        const Point2D& portal1Pos, const Point2D& portal2Pos, float width, float height, 
        const Colour& colour, double baseTerminationTimeInSecs);
    ~PortalProjectile();

    ProjectileType GetType() const { return Projectile::PortalBlobProjectile; }

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }
    float GetDamage() const { return 0.0f; }

    bool CanCollideWithBlocks() const { return false; }
    bool CanCollideWithBall() const { return true; }
    bool CanCollideWithProjectiles() const { return true; }

    void BallCollisionOccurred(GameBall* ball);
    void ProjectileCollisionOccurred(Projectile* projectile);

    void Tick(double, const GameModel&) {}
    bool ModifyLevelUpdate(double dT, GameModel& model);

    BoundingLines BuildBoundingLines() const;

    const ColourRGBA& GetColour() const { return this->colour; }
    const PortalProjectile* GetSiblingPortal() const { return this->sibling; }

    bool IsTerminating() const;

private:
    static const int NO_TERMINATION_TIME = INT_MIN;
    static const float BOUNDS_COEFF;

    PortalProjectile* sibling; // NOT owned by this, only referenced

    ColourRGBA colour;
    bool ballHasGoneThroughBefore;

    double baseTeriminationTime; // The base time it will take (without resets) for the portal to terminate itself
    double terminationCountdown; // The portal counts down to its (and its sibling's) termination

    PortalProjectile(const Point2D& spawnLoc, float width, float height, 
        const Colour& colour, double baseTerminationTimeInSecs);

    void SetSibling(PortalProjectile* sibling);
    void ResetTeriminationCountdown();

    DISALLOW_COPY_AND_ASSIGN(PortalProjectile);
};

inline bool PortalProjectile::IsTerminating() const {
    // If the ball no longer has a sibling OR has gone through both portals OR
    // has a finished the termination countdown, then we are terminating it
    return this->sibling == NULL || 
        (this->ballHasGoneThroughBefore && this->sibling->ballHasGoneThroughBefore) || 
        (this->terminationCountdown != NO_TERMINATION_TIME && this->terminationCountdown <= 0);
}

inline void PortalProjectile::SetSibling(PortalProjectile* sibling) {
    assert(sibling != NULL);
    this->sibling = sibling;
}

#endif // __PORTALPROJECTILE_H__