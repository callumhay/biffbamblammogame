/**
 * Collision.cpp
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

#include "Collision.h"

using namespace Collision;

#define BALL_COLLISION_SAMPLING_INV_AMT 0.1f

bool Circle2D::CollideGetCollisionPt(double dT, const Collision::Circle2D& c, const Vector2D& velocity,
                                     Point2D& collisionPt, float& collisionDist, float& moveVecDist) const {
    assert(c.Radius() > 0);

    

    Vector2D moveVec = dT * velocity;
    if (moveVec.IsZero()) {
        return false;
    }

    moveVecDist = moveVec.Magnitude();
    float dist = (c.Center() - this->Center()).Magnitude();
    float sumRadii = (c.Radius() + this->Radius());
    dist -= sumRadii;
    if (moveVecDist < dist) {
        return false;
    }

    Vector2D moveDir = moveVec / moveVecDist;
    Vector2D cVec = this->Center() - c.Center();
    float d = Vector2D::Dot(moveDir, cVec);

    if (d <= 0) {
        return false;
    }

    float f = cVec.SqrMagnitude() - (d*d);
    float sqrSumRadii = sumRadii * sumRadii;
    if (f >= sqrSumRadii) {
        return false;
    }

    float t = sqrSumRadii - f;
    if (t < 0) {
        return false;
    }

    collisionDist = d - sqrt(t);
    if (moveVecDist < collisionDist) {
        return false;
    }

    collisionPt = c.Center() + collisionDist * moveDir;
    return true;
}

bool Circle2D::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
                       Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& cCenterAtCollision) const {

    float collisionDist, moveVecDist;
    if (!this->CollideGetCollisionPt(dT, c, velocity, cCenterAtCollision, collisionDist, moveVecDist)) {
        return false;
    }

    timeUntilCollision = collisionDist / velocity.Magnitude();
    n = Vector2D::Normalize(cCenterAtCollision - this->Center());
    
    Vector2D perpToNormal(-n[1], n[0]);
    Point2D linePos = cCenterAtCollision - c.Radius() * n;
    collisionLine = Collision::LineSeg2D(linePos - perpToNormal, linePos + perpToNormal);

    return true;
}

bool Circle2D::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
                       Collision::LineSeg2D& collisionLine, double& timeUntilCollision, 
                       Point2D& cCenterAtCollision, Point2D& thisCenterAtCollision, 
                       const Vector2D& thisCircleVelocity) const {

    bool zeroThisCircleVelocity = thisCircleVelocity.IsZero();
    if (zeroThisCircleVelocity) {
       // Simpler solution if this circle isn't moving
       thisCenterAtCollision = this->Center();
       return this->Collide(dT, c, velocity, n, collisionLine, timeUntilCollision, cCenterAtCollision);
    }

    bool zeroOtherBallVelocity = velocity.IsZero();
    if (zeroOtherBallVelocity) {
        return false;
    }
    
    // The solution to a two moving circle problem is pretty much the same as a 
    // single moving circle colliding with a static circle, but we need to change the frame of
    // reference to a single circle...
    // See: http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2
    Vector2D relativeVel = velocity - thisCircleVelocity;
    float collisionDist, moveVecDist;
    if (!this->CollideGetCollisionPt(dT, c, relativeVel, cCenterAtCollision, collisionDist, moveVecDist)) {
        return false;
    }

    float fract = 0.0f;
    if (moveVecDist >= EPSILON) {
        // Move back to the original frame of reference...
        fract = collisionDist / moveVecDist;
    }

    // Determine the non-relative positions of both circles at the time of collision
    cCenterAtCollision    = c.Center() + fract * dT * velocity;
    thisCenterAtCollision = this->Center() + fract * dT * thisCircleVelocity;
    timeUntilCollision    = NumberFuncs::SignOf(fract) * (cCenterAtCollision - c.Center()).Magnitude() / velocity.Magnitude();

    n = Vector2D::Normalize(cCenterAtCollision - thisCenterAtCollision);
    Vector2D perpToNormal(-n[1], n[0]);
    Point2D linePos = cCenterAtCollision - c.Radius() * n;
    collisionLine = Collision::LineSeg2D(linePos - perpToNormal, linePos + perpToNormal);

    return true;
}

#ifdef _DEBUG
void AABB2D::DebugDraw() {
    glPushAttrib(GL_CURRENT_BIT);

    glColor4f(0, 1, 1, 1);

    glBegin(GL_LINE_LOOP);
    glVertex2f(this->minCoord[0], this->minCoord[1]);
    glVertex2f(this->maxCoord[0], this->minCoord[1]);
    glVertex2f(this->maxCoord[0], this->maxCoord[1]);
    glVertex2f(this->minCoord[0], this->maxCoord[1]);
    glEnd();

    glPopAttrib();
}
#endif