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

#include "PortalProjectile.h"
#include "GameBall.h"
#include "GameEventManager.h"

const float PortalProjectile::BOUNDS_COEFF = 0.9f;

std::pair<PortalProjectile*, PortalProjectile*> 
PortalProjectile::BuildSiblingPortalProjectiles(const Point2D& portal1Pos, const Point2D& portal2Pos, 
                                                float width, float height, const Colour& colour, 
                                                double baseTerminationTimeInSecs) {

    // NOTE: We only give the termination time to ONE of the portals (so they aren't both
    // counting down, which is would just be unnecessary and odd)
    PortalProjectile* portal1 = new PortalProjectile(portal1Pos, width, height, colour, baseTerminationTimeInSecs);
    PortalProjectile* portal2 = new PortalProjectile(portal2Pos, width, height, colour, NO_TERMINATION_TIME);
    portal1->SetSibling(portal2);
    portal2->SetSibling(portal1);

    return std::make_pair(portal1, portal2);
}

PortalProjectile::~PortalProjectile() {
    this->sibling = NULL;
}

PortalProjectile::PortalProjectile(const Point2D& spawnLoc, float width, float height, 
                                   const Colour& colour, double baseTerminationTimeInSecs) : 
Projectile(spawnLoc, width, height), sibling(NULL), colour(colour, 1.0f), 
ballHasGoneThroughBefore(false), baseTeriminationTime(baseTerminationTimeInSecs), 
terminationCountdown(baseTerminationTimeInSecs) {

    // This is VERY necessary. Also make sure to call the two argument setter or else
    // there will be assertion errors.
    this->SetVelocity(Vector2D(0,0), 0);
}

bool PortalProjectile::ModifyLevelUpdate(double dT, GameModel&) {

    if (this->terminationCountdown != NO_TERMINATION_TIME) {
        this->terminationCountdown -= dT;
    }

    if (this->IsTerminating()) {
        // Make sure we tell the sibling portal (if it still exists) that this portal has terminated
        if (this->sibling != NULL) {
            this->sibling->sibling = NULL;
        }
        return true;
    }

    return false;
}

void PortalProjectile::BallCollisionOccurred(GameBall* ball) {
    // Don't let the ball collide again if it's already gone through both portals
    // or if this portal doesn't have a sibling
    if (this->IsTerminating()) {
        return;
    }

    // Tell the ball that the sibling portal was the last thing it collided with
    ball->SetLastThingCollidedWith(this->sibling);

    // EVENT: The ball has officially entered the portal...
    GameEventManager::Instance()->ActionBallPortalProjectileTeleport(*ball, *this);

    // Teleport the ball to the sibling portal block
    Vector2D centerToBall = ball->GetCenterPosition2D() - this->GetPosition();
    ball->SetCenterPosition(this->sibling->GetPosition() + centerToBall);

    // Tell the sibling that it's last ball collision is now
    //this->sibling->timeOfLastBallCollision = BlammoTime::GetSystemTimeInMillisecs();

    this->ballHasGoneThroughBefore = true;
    this->ResetTeriminationCountdown();
}

void PortalProjectile::ProjectileCollisionOccurred(Projectile* projectile) {
    assert(projectile != NULL && projectile != this);

    // If this portal is terminating or if the projectile is another portal
    // then we ignore the collision
    if (this->IsTerminating() || projectile->GetType() == Projectile::PortalBlobProjectile) {
        return;
    }

    projectile->SetLastThingCollidedWith(this->sibling);
    
    // EVENT: The projectile has officially entered the portal...
    GameEventManager::Instance()->ActionProjectilePortalProjectileTeleport(*projectile, *this);

    // Teleport the projectile to the sibling portal block
    Vector2D centerToProjectile = projectile->GetPosition() - this->GetPosition();
    projectile->SetPosition(this->sibling->GetPosition() + centerToProjectile);
}

BoundingLines PortalProjectile::BuildBoundingLines() const {
 
    static const Vector2D UP_DIR(0,1);
    static const Vector2D RIGHT_DIR(1,0);

    const Vector2D HALF_UP_VEC    = BOUNDS_COEFF*this->GetHalfHeight()*UP_DIR;
    const Vector2D HALF_RIGHT_VEC = BOUNDS_COEFF*this->GetHalfWidth()*RIGHT_DIR;

    Point2D topRight = this->GetPosition() + HALF_UP_VEC + HALF_RIGHT_VEC;
    Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
    Point2D topLeft = this->GetPosition() + HALF_UP_VEC - HALF_RIGHT_VEC;
    Point2D bottomLeft = topLeft - this->GetHeight()*UP_DIR;

    static const int NUM_BOUNDS = 4;
    Collision::LineSeg2D bounds[NUM_BOUNDS];
    Vector2D norms[NUM_BOUNDS];

    bounds[0] = Collision::LineSeg2D(topLeft, bottomLeft);
    bounds[1] = Collision::LineSeg2D(topRight, bottomRight);
    bounds[2] = Collision::LineSeg2D(topLeft, topRight);
    bounds[3] = Collision::LineSeg2D(bottomLeft, bottomRight);

    return BoundingLines(NUM_BOUNDS, bounds, norms);
}

void PortalProjectile::ResetTeriminationCountdown() {
    if (this->terminationCountdown != NO_TERMINATION_TIME) {
        // Reset the termination countdown on this portal to the beginning
        this->terminationCountdown = this->baseTeriminationTime;
    }
    else {
        // The countdown is in the sibling, reset it there instead
        assert(this->sibling->terminationCountdown != NO_TERMINATION_TIME);
        this->sibling->terminationCountdown = this->sibling->baseTeriminationTime;
    }
}