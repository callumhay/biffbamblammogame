/**
 * Collision.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Collision.h"

using namespace Collision;

#define BALL_COLLISION_SAMPLING_INV_AMT 0.1f

bool Circle2D::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
                       Collision::LineSeg2D& collisionLine, double& timeUntilCollision) const {

    assert(c.Radius() > 0);

    Vector2D sampleIncDist;
    double sampleIncTime;
    int numCollisionSamples;

    bool zeroVelocity = (velocity == Vector2D(0.0f, 0.0f));

    if (zeroVelocity) {
       numCollisionSamples = 1;
    }
    else {
       // Calculate the number of samples required to make sure that the increment distance is
       // less than a reasonable fraction of the radius of the circle
       numCollisionSamples = ceil(ceil(dT / (BALL_COLLISION_SAMPLING_INV_AMT * c.Radius()) * velocity.Magnitude()));
       numCollisionSamples = std::min<int>(40, std::max<int>(1, numCollisionSamples + 1));
    }

    // Figure out the distance along the vector traveled since the last collision
    // to take each sample at...
    sampleIncDist = dT * velocity / static_cast<float>(numCollisionSamples);
    sampleIncTime = dT / static_cast<double>(numCollisionSamples);

    Circle2D sampleCircle = c;
    double currTimeUntilCollision = 0.0;

    // Go through all of the samples starting with the first, moving towards the circle's current location, 
    // when a collision is found we exit
    bool isCollision = false;
    for (int i = 0; i < numCollisionSamples; i++) {
        
        if (Collision::IsCollision(sampleCircle, *this)) {
            // Calculate the normal and line of collision...
            Vector2D thisToBallVec = sampleCircle.Center() - this->Center();
            n = Vector2D::Normalize(thisToBallVec);
            
            // Make the collision line tangent to the circle
            Vector2D perpToNormal(-n[1], n[0]);
            Point2D approxCollisionPt = sampleCircle.Center() - sampleCircle.Radius() * n;
            collisionLine = Collision::LineSeg2D(approxCollisionPt - perpToNormal, approxCollisionPt + perpToNormal);

            isCollision = true;
            break;
        }
        sampleCircle.SetCenter(sampleCircle.Center() + sampleIncDist);
        currTimeUntilCollision += sampleIncTime;
    }

    timeUntilCollision = currTimeUntilCollision;
    return isCollision;
}

bool Circle2D::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
                       Collision::LineSeg2D& collisionLine, double& timeUntilCollision, const Vector2D& thisCircleVelocity) const {

    assert(c.Radius() > 0);

    bool zeroThisCircleVelocity = (thisCircleVelocity == Vector2D(0.0f, 0.0f));
    if (zeroThisCircleVelocity) {
       // Simpler solution if the line isn't moving
       return this->Collide(dT, c, velocity, n, collisionLine, timeUntilCollision);
    }

    bool zeroBallVelocity = (velocity == Vector2D(0.0f, 0.0f));
    int numBallCollisionSamples = 1;
    int numLineCollisionSamples = 1;

    float minimumRadius = std::min<float>(c.Radius(), this->Radius());

    if (!zeroBallVelocity) {
       // Calculate the number of samples required to make sure that the increment distance
       // less than or equal to a fraction of the radius of the circle
       numBallCollisionSamples = ceil(ceil(dT / (BALL_COLLISION_SAMPLING_INV_AMT * minimumRadius) * velocity.Magnitude()));
       numBallCollisionSamples = std::min<int>(40, std::max<int>(1, numBallCollisionSamples + 1));
    }
    if (!zeroThisCircleVelocity) {
       // Calculate the number of samples required to make sure that the increment distance
       // less than or equal to some reasonable delta distance (a fraction of the radius of the circle)...
       numLineCollisionSamples = ceil(ceil(dT / (BALL_COLLISION_SAMPLING_INV_AMT * minimumRadius) * thisCircleVelocity.Magnitude()));
       numLineCollisionSamples = std::min<int>(40, std::max<int>(1, numLineCollisionSamples + 1));
    }

    int maxCollisionSamples = std::max<int>(numBallCollisionSamples, numLineCollisionSamples);

    // Figure out the distance along the vector traveled since the last collision
    // to take each sample at...
    Vector2D sampleIncDistForBall = dT * velocity / static_cast<float>(maxCollisionSamples);
    Vector2D sampleIncDistForLine = dT * thisCircleVelocity / static_cast<float>(maxCollisionSamples);
    double sampleIncTime = dT / static_cast<double>(maxCollisionSamples);

    Circle2D currSampleBallCircle = c;
    Circle2D currSampleThisCircle = *this;
    double currTimeUntilCollision = 0.0;

    // Move the two circles in the directions they're traveling over the course of the dT
    // in order to find out when a collision (if any) occurred
    bool isCollision = false;
    for (int i = 0; i < maxCollisionSamples; i++) {

       if (Collision::IsCollision(currSampleBallCircle, currSampleThisCircle)) {
           
           // Calculate the normal and line of collision...
           Vector2D thisToBallVec = currSampleBallCircle.Center() - currSampleThisCircle.Center();
           n = Vector2D::Normalize(thisToBallVec);

           // Make the collision line tangent to the circle
           Vector2D perpToNormal(-n[1], n[0]);
           Point2D approxCollisionPt = currSampleBallCircle.Center() - currSampleBallCircle.Radius() * n;
           collisionLine = Collision::LineSeg2D(approxCollisionPt - perpToNormal, approxCollisionPt + perpToNormal);

           isCollision = true;
           break;
       }

       currSampleBallCircle.SetCenter(currSampleBallCircle.Center() + sampleIncDistForBall);
       currSampleThisCircle.SetCenter(currSampleThisCircle.Center() + sampleIncDistForLine);
       currTimeUntilCollision += sampleIncTime;
    }

    timeUntilCollision = currTimeUntilCollision;
    return isCollision;
}