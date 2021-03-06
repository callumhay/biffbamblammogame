/**
 * PortalProjectile.cpp
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

#include "PortalProjectile.h"
#include "GameBall.h"
#include "GameEventManager.h"

const float PortalProjectile::BOUNDS_COEFF = 0.7f;

// Fraction of the base termination time where this portal is considered to be "close to" terminating
const double PortalProjectile::CLOSE_TO_TERMINATING_FRACTION = 0.33;

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

    if (baseTerminationTimeInSecs != NO_TERMINATION_TIME) {
        this->SetupAlphaFlickerAnim(CLOSE_TO_TERMINATING_FRACTION*baseTerminationTimeInSecs);
    }

    // This is VERY necessary. Also make sure to call the two argument setter or else
    // there will be assertion errors.
    this->SetVelocity(Vector2D(0,0), 0);
}

bool PortalProjectile::ModifyLevelUpdate(double dT, GameModel&) {

    if (this->baseTeriminationTime != NO_TERMINATION_TIME) {
        this->terminationCountdown -= dT;

        // Animate the close-to-termination alpha for both this portal and its sibling
        if (!this->IsTerminating()) {
            if (this->terminationCountdown <= CLOSE_TO_TERMINATING_FRACTION*this->baseTeriminationTime) {

                this->alphaFlickerAnim.Tick(dT);
                this->colour[3] = this->sibling->colour[3] = this->alphaFlickerAnim.GetInterpolantValue();
            }
            else {
                this->colour[3] = this->sibling->colour[3] = 1.0f;
            }
        }
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

void PortalProjectile::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir, 
                                                   std::list<Collision::Ray2D>& rays) const {

    if (this->IsTerminating()) {
        return;
    }

    // Find position difference between the point and center...
    Vector2D toHitVec = hitPoint - this->GetPosition();
   
    // The new ray is simply the old one coming out of the sibling portal
    Point2D newPosition = this->sibling->GetPosition() + toHitVec;
    rays.push_back(Collision::Ray2D(newPosition, impactDir));
}

BoundingLines PortalProjectile::BuildBoundingLines() const {
 
    static const Vector2D UP_DIR(0,1);
    static const Vector2D RIGHT_DIR(1,0);

    const Vector2D HALF_UP_VEC    = BOUNDS_COEFF*this->GetHalfHeight()*UP_DIR;
    const Vector2D HALF_RIGHT_VEC = BOUNDS_COEFF*this->GetHalfWidth()*RIGHT_DIR;

    Point2D topRight    = this->GetPosition() + HALF_UP_VEC + HALF_RIGHT_VEC;
    Point2D bottomRight = this->GetPosition() - HALF_UP_VEC + HALF_RIGHT_VEC;
    Point2D topLeft     = this->GetPosition() + HALF_UP_VEC - HALF_RIGHT_VEC;
    Point2D bottomLeft  = this->GetPosition() - HALF_UP_VEC - HALF_RIGHT_VEC;

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
        this->alphaFlickerAnim.ResetToStart();
    }
    else {
        // The countdown is in the sibling, reset it there instead
        assert(this->sibling->terminationCountdown != NO_TERMINATION_TIME);
        this->sibling->terminationCountdown = this->sibling->baseTeriminationTime;
        this->sibling->alphaFlickerAnim.ResetToStart();
    }
}

void PortalProjectile::SetupAlphaFlickerAnim(double totalFlickerTime) {
    static const double FLASH_FREQ = 50;
    int numFlashes = static_cast<int>(totalFlickerTime*FLASH_FREQ);
    if (numFlashes % 2 == 0) {
        numFlashes++;
    }
    double timePerFlash = totalFlickerTime/numFlashes;

    std::vector<double> timeVals(numFlashes, 0.0);
    timeVals[0] = 0.0;
    for (int i = 1; i < numFlashes; i++) {
        timeVals[i] = timeVals[i-1] + timePerFlash;
    }

    std::vector<float> alphaVals;
    alphaVals.reserve(numFlashes);
    alphaVals.push_back(1.0f);
    for (int i = 1; i < numFlashes; i += 2) {
        alphaVals.push_back(0.01f); 
        alphaVals.push_back(1.0f);
    }

    this->alphaFlickerAnim.SetLerp(timeVals, alphaVals);
    this->alphaFlickerAnim.SetRepeat(false);
}