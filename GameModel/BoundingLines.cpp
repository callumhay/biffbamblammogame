/**
 * BoundingLines.cpp
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

#include "BoundingLines.h"
#include "LevelPiece.h"

const float BoundingLines::BALL_INSIDE_OUTSIDE_DIST_DIVISOR = 7.0f;
const float BoundingLines::BALL_COLLISION_SAMPLING_INV_AMT  = 0.1f;

BoundingLines::BoundingLines(const std::vector<Collision::LineSeg2D>& lines,
                             const std::vector<Vector2D>& norms) : lines(lines), normals(norms) {
	assert(lines.size() == norms.size());
    this->onInside.resize(lines.size(), false);
}

BoundingLines::BoundingLines(const std::vector<Collision::LineSeg2D>& lines,
                             const std::vector<Vector2D>& norms,
                             const std::vector<bool>& onInside) : 
lines(lines), normals(norms), onInside(onInside) {
    assert(lines.size() == norms.size());
    assert(lines.size() == onInside.size());
}

BoundingLines::BoundingLines(int numLines, const Collision::LineSeg2D lineArray[], 
                             const Vector2D normArray[], const bool onInsideArray[]) :
lines(lineArray, lineArray + numLines), 
normals(normArray, normArray + numLines), 
onInside(onInsideArray, onInsideArray + numLines) {
}

BoundingLines::BoundingLines(int numLines, const Collision::LineSeg2D lineArray[], const Vector2D normArray[]) :
lines(lineArray, lineArray + numLines), 
normals(normArray, normArray + numLines), 
onInside(numLines, false) {
}

BoundingLines::BoundingLines(const Collision::AABB2D& aabb) {
    Point2D aabbTopLeft(aabb.GetMin()[0], aabb.GetMax()[1]);
    Point2D aabbBottomRight(aabb.GetMax()[0], aabb.GetMin()[1]);

    // Top
    this->lines.push_back(Collision::LineSeg2D(aabbTopLeft, aabb.GetMax()));
    this->normals.push_back(Vector2D(0, 1));

    // Left
    this->lines.push_back(Collision::LineSeg2D(aabb.GetMin(), aabbTopLeft));
    this->normals.push_back(Vector2D(-1, 0));

    // Right
    this->lines.push_back(Collision::LineSeg2D(aabb.GetMax(), aabbBottomRight));
    this->normals.push_back(Vector2D(1, 0));

    // Bottom
    this->lines.push_back(Collision::LineSeg2D(aabb.GetMin(), aabbBottomRight));
    this->normals.push_back(Vector2D(0, -1));

    this->onInside.resize(4, false);
}

BoundingLines::BoundingLines(const BoundingLines& copy) :
lines(copy.lines), normals(copy.normals), onInside(copy.onInside) {
}

BoundingLines::~BoundingLines() {
}

void BoundingLines::AddBound(const Collision::LineSeg2D& line, const Vector2D& norm, bool onInside) {
    this->lines.push_back(line);
    this->normals.push_back(norm);
    this->normals.back().Normalize();
    this->onInside.push_back(onInside);
}

void BoundingLines::AddBounds(const BoundingLines& bounds) {
    this->lines.insert(this->lines.end(), bounds.lines.begin(), bounds.lines.end());
    this->normals.insert(this->normals.end(), bounds.normals.begin(), bounds.normals.end());
    this->onInside.insert(this->onInside.end(), bounds.onInside.begin(), bounds.onInside.end());
}

Collision::AABB2D BoundingLines::GenerateAABBFromLines() const {
    if (this->lines.empty()) {
        assert(false);
        return Collision::AABB2D();
    }

	std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin();
	const Collision::LineSeg2D& firstLine = *thisIter;

	float minX = firstLine.P1()[0];
	float maxX = firstLine.P1()[0];
	float minY = firstLine.P1()[1];
	float maxY = firstLine.P1()[1];

	while (thisIter != this->lines.end()) {
		const Collision::LineSeg2D& currThisLine = *thisIter;

		// First point on the line...
		if (currThisLine.P1()[0] < minX) {
			minX = currThisLine.P1()[0];
		}
		if (currThisLine.P1()[0] > maxX) {
			maxX = currThisLine.P1()[0];
		}
		if (currThisLine.P1()[1] < minY) {
			minY = currThisLine.P1()[1];
		}
		if (currThisLine.P1()[1] > maxY) {
			maxY = currThisLine.P1()[1];
		}

		// Second point on the line...
		if (currThisLine.P2()[0] < minX) {
			minX = currThisLine.P2()[0];
		}
		if (currThisLine.P2()[0] > maxX) {
			maxX = currThisLine.P2()[0];
		}
		if (currThisLine.P2()[1] < minY) {
			minY = currThisLine.P2()[1];
		}
		if (currThisLine.P2()[1] > maxY) {
			maxY = currThisLine.P2()[1];
		}

		++thisIter;
	}

	return Collision::AABB2D(Point2D(minX, minY), Point2D(maxX, maxY));
}

Collision::Circle2D BoundingLines::GenerateCircleFromLines() const {
    if (this->lines.empty()) {
        return Collision::Circle2D(Point2D(0,0), 0.0f);
    }

	std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin();
	const Collision::LineSeg2D& firstLine = *thisIter;

	Point2D minX = firstLine.P1();
	Point2D maxX = firstLine.P1();
	Point2D minY = firstLine.P1();
	Point2D maxY = firstLine.P1();

    // Calculate the min and max along the x and y axes
	while (thisIter != this->lines.end()) {
		const Collision::LineSeg2D& currThisLine = *thisIter;
		// First point on the line...
		if (currThisLine.P1()[0] < minX[0]) {
			minX = currThisLine.P1();
		}
		if (currThisLine.P1()[0] > maxX[0]) {
			maxX = currThisLine.P1();
		}
		if (currThisLine.P1()[1] < minY[1]) {
			minY = currThisLine.P1();
		}
		if (currThisLine.P1()[1] > maxY[1]) {
			maxY = currThisLine.P1();
		}

		// Second point on the line...
		if (currThisLine.P2()[0] < minX[0]) {
			minX = currThisLine.P2();
		}
		if (currThisLine.P2()[0] > maxX[0]) {
			maxX = currThisLine.P2();
		}
		if (currThisLine.P2()[1] < minY[1]) {
			minY = currThisLine.P2();
		}
		if (currThisLine.P2()[1] > maxY[1]) {
			maxY = currThisLine.P2();
		}

		++thisIter;
    }
    
    // Compute max squared distances along x and y axes
    float dist2x = Vector2D::Dot(maxX - minX, maxX - minX);
    float dist2y = Vector2D::Dot(maxY - minY, maxY - minY);

    Point2D min = minX;
    Point2D max = maxX;

    if (dist2y > dist2x) {
        min = minY;
        max = maxY;
    }

    Point2D center =  0.5f * (min + Vector2D(max[0], max[1]));
    float radius = Vector2D::Dot(max - center, max - center);
    assert(radius > 0.0);
    radius = sqrt(radius);
    
    return Collision::Circle2D(center, radius); 
}

/**
 * Test collision between the bounding lines of this and the given circle.
 * Return: The average normal for the lines that the given circle collided with in the value n. 
 * Also returns the distance from the center of the circle to the line (negative d means the circle is
 * inside the line, positive means outside - the outward normal pointing direction).
 * The boolean return value will be true if one or more collisions occurred, false otherwise.
 */
bool BoundingLines::Collide(double dT, const Collision::Circle2D& circle, const Vector2D& velocity, Vector2D& n, 
							Collision::LineSeg2D& collisionLine, int& collisionLineIdx, double& timeUntilCollision,
                            Point2D& cPointOfCollision) const {

    assert(circle.Radius() > 0);
    
    float velocityMag  = velocity.Magnitude();
    Vector2D nVelocity(0,0);
    bool zeroVelocity = false;
    if (velocityMag > EPSILON) {
        nVelocity = velocity / velocityMag;
    }
    else {
        zeroVelocity = true;
    }

    Collision::LineSeg2D circleVelLine(circle.Center(), circle.Center() + dT * velocity);
    Collision::Ray2D circleVelRay(circle.Center(), nVelocity);
    const float sqrRadius = circle.Radius() * circle.Radius();
    
    float rayT, lineT;
    Point2D point1, point2, pointC, a, tempPt2D;
    Vector2D tempDiff;
    bool collisionHappened;

    std::vector<size_t> collisionLineIdxs;
    collisionLineIdxs.reserve(this->GetNumLines());
    std::vector<Point2D> closestPts;
    closestPts.reserve(this->GetNumLines());
    
    bool isCollision = false;
    int closestLineIdx = -1;
    double minTimeUntilCollision = std::numeric_limits<double>::max();

    // Solution for moving circle to static-line collision, found here:
    // http://ericleong.me/research/circle-line
    for (int lineIdx = 0; lineIdx < static_cast<int>(this->lines.size()); ++lineIdx) {
        
        const Collision::LineSeg2D& currBoundsLine = this->lines[lineIdx];
        
        // Start by finding the closest point on the bounds line to the center of the circle...
        Collision::ClosestPoint(circle.Center(), currBoundsLine, point1);
        if ((circle.Center() - point1).SqrMagnitude() <= sqrRadius) {
            // The circle is already colliding with the current bounding line...
            
            collisionLineIdxs.push_back(lineIdx);
            closestPts.push_back(point1);
            closestLineIdx = lineIdx;
            minTimeUntilCollision = 0.0;
            isCollision = true;

            continue;
        }
        
        collisionHappened = false;
        if (Collision::IsCollision(circleVelRay, currBoundsLine, rayT, lineT)) {
            if (rayT <= 0.0f) {
                // No collision, the line is behind the circle velocity ray
                continue;
            }

            a = circleVelRay.GetPointAlongRayFromOrigin(rayT);
            // Check if the ray hits the within the line segment and the velocity vector
            if (lineT >= 0.0f && lineT <= 1.0f && rayT <= velocityMag*dT) {
                // There's a collision
                collisionHappened = true;
            }
            else {
                // The circle velocity ray doesn't directly intersect the bounding line segment, 
                // but a collision can still happen with the outer capsule formed by the moving circle...
                Collision::ClosestPoint(circleVelLine.P2(), currBoundsLine, tempPt2D);
                if ((circleVelLine.P2() - tempPt2D).SqrMagnitude() <= sqrRadius) {
                    collisionHappened = true;
                }
                else {
                    Collision::ClosestPoint(currBoundsLine.P1(), circleVelLine, tempPt2D);
                    if ((currBoundsLine.P1() - tempPt2D).SqrMagnitude() <= sqrRadius) {
                        collisionHappened = true;
                    }
                    else {

                        Collision::ClosestPoint(currBoundsLine.P2(), circleVelLine, tempPt2D);
                        if ((currBoundsLine.P2() - tempPt2D).SqrMagnitude() <= sqrRadius) {
                            collisionHappened = true;
                        }
                    }
                }
            }

            if (collisionHappened) {

                // Calculate point2, the position of the circle at collision
                point2 = a - (circle.Radius() * (((a - circle.Center()).Magnitude()) / (point1 - circle.Center()).Magnitude()) * (nVelocity / velocityMag));

                // Calculate pointC, the contact point on the line at collision.
                // Check to see if pointC is actually on the bounding line, if not then the circle must have collided
                // with an end point of the bounding line...

                double tempTime;
                if (Collision::ClosestPointMustBeOnSegment(point2, currBoundsLine, pointC)) {

                    tempTime = (point2 - circle.Center()).Magnitude() / velocityMag;
                }
                else {
                    // Deal with the special case of end point collisions...

                    // Find the closest point on the movement vector to the end point that the circle collided with
                    Collision::ClosestPoint(pointC, circleVelLine, tempPt2D);

                    // Use Pythagorean theorem to solve for where the collision occurred
                    tempDiff = pointC - tempPt2D;
                    tempPt2D -= sqrt(std::max<float>(0.0f, (sqrRadius - tempDiff.SqrMagnitude()))) * nVelocity;

                    tempTime = (tempPt2D - circle.Center()).Magnitude() / velocityMag;
                }

                if (tempTime < minTimeUntilCollision) {
                    minTimeUntilCollision = tempTime;
                    closestLineIdx = lineIdx;
                }

                collisionLineIdxs.push_back(lineIdx);
                closestPts.push_back(pointC);
                isCollision = true;
            }
        }
        else {
            // It looks like the circle velocity ray is parallel to the bounding line,
            // The circle has to collide with either one or the other end in order for their to be a collision
            Collision::ClosestPoint(currBoundsLine.P1(), circleVelLine, tempPt2D);
            if ((currBoundsLine.P1() - tempPt2D).SqrMagnitude() <= sqrRadius) {
                collisionHappened = true;
                pointC = currBoundsLine.P1();
            }
            else {

                Collision::ClosestPoint(currBoundsLine.P2(), circleVelLine, tempPt2D);
                if ((currBoundsLine.P2() - tempPt2D).SqrMagnitude() <= sqrRadius) {
                    collisionHappened = true;
                    pointC = currBoundsLine.P2();
                }
            }

            if (collisionHappened) {
                // Deal with the special case of end point collisions...

                // Find the closest point on the movement vector to the end point that the circle collided with
                Collision::ClosestPoint(pointC, circleVelLine, tempPt2D);

                // Use Pythagorean theorem to solve for where the collision occurred
                tempDiff = pointC - tempPt2D;
                tempPt2D -= sqrt(std::max<float>(0.0f, (sqrRadius - tempDiff.SqrMagnitude()))) * nVelocity;

                double tempTime = (tempPt2D - circle.Center()).Magnitude() / velocityMag;
                if (tempTime < minTimeUntilCollision) {
                    minTimeUntilCollision = tempTime;
                    closestLineIdx = lineIdx;
                }
 
                collisionLineIdxs.push_back(lineIdx);
                closestPts.push_back(pointC);
                isCollision = true;
            }
        }
    }

    assert(minTimeUntilCollision >= 0.0);
    if (!isCollision || minTimeUntilCollision > dT) {
        return false;
    }

    assert(!collisionLineIdxs.empty());

    timeUntilCollision = minTimeUntilCollision;
    cPointOfCollision = circle.Center() + timeUntilCollision * velocity;

    if (zeroVelocity || collisionLineIdxs.size() == 1) {
        n = this->normals[collisionLineIdxs.front()];
        collisionLine = this->lines[collisionLineIdxs.front()];
        collisionLineIdx = collisionLineIdxs.front();
    }
    else {

        // We need to figure out which line got hit first
        float smallestInsideDist  = FLT_MAX;
        int insideIdx = -1;

        float smallestOutsideDist = FLT_MAX;
        int outsideIdx = -1;

        bool hasInside = false;
        for (int i = 0; i < static_cast<int>(collisionLineIdxs.size()); i++) {
            hasInside |= this->onInside[collisionLineIdxs[i]];
        }

        assert(collisionLineIdxs.size() == closestPts.size());

        if (hasInside) {

            for (int i = 0; i < static_cast<int>(collisionLineIdxs.size()); i++) {

                int lineIdx = collisionLineIdxs[i];
                const Collision::LineSeg2D& currLine = this->lines[lineIdx];
                const Vector2D& currNormal = this->normals[lineIdx];
                float d    = Vector2D::Dot(currNormal, Vector2D(currLine.P1()[0], currLine.P1()[1]));
                float dist = Vector2D::Dot(currNormal, Vector2D(cPointOfCollision[0], cPointOfCollision[1])) - d;
                float absDist = fabs(dist);

                if (this->onInside[lineIdx]) {
                    if (absDist < smallestInsideDist) {
                        smallestInsideDist = absDist;
                        insideIdx = lineIdx;
                    }
                }
                else {
                    if (absDist < smallestOutsideDist) {
                        smallestOutsideDist = absDist;
                        outsideIdx = lineIdx;
                    }
                }
            }
        }
        else {
            for (int i = 0; i < static_cast<int>(collisionLineIdxs.size()); i++) {

                int lineIdx = collisionLineIdxs[i];
                const Point2D& closestPt = closestPts[i];
                float sqrDist = Point2D::SqDistance(cPointOfCollision, closestPt);

                assert(!this->onInside[lineIdx]);
                if (sqrDist < smallestOutsideDist) {

                    if (outsideIdx >= 0 && fabs(sqrDist - smallestOutsideDist) <= EPSILON) {
                        // Really close call... choose the normal that is closest to the opposite of the ball's velocity
                        const Vector2D& currNormal  = this->normals[lineIdx];
                        const Vector2D& otherNormal = this->normals[outsideIdx];

                        if (acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(nVelocity, currNormal)))) >
                            acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(nVelocity, otherNormal))))) {

                            smallestOutsideDist = sqrDist;
                            outsideIdx = lineIdx;
                        }
                    }
                    else {
                        smallestOutsideDist = sqrDist;
                        outsideIdx = lineIdx;
                    }
                }
            }
        }

#define DO_ASSIGNMENT(idx) n = this->normals[idx]; collisionLine = this->lines[idx]; collisionLineIdx = idx;

        // If only an inside or an outside was found then return that
        if (outsideIdx == -1) {
            assert(insideIdx != -1);
            DO_ASSIGNMENT(insideIdx);
        }
        else if (insideIdx == -1) {
            assert(outsideIdx != -1);
            DO_ASSIGNMENT(outsideIdx);
        }
        else {
            // There is an outside and an inside found in the tie... 

            // Check to see if the ball is in the half-plane of the outside boundary, also
            // if they are close together then we favour the outside boundary over the inside...
            const float COLLISION_DISTANCE_EPSILON = circle.Radius() / BALL_INSIDE_OUTSIDE_DIST_DIVISOR;
            Vector2D toBallFromOutsideLine = cPointOfCollision - this->lines[outsideIdx].P1();
            if (Vector2D::Dot(toBallFromOutsideLine, this->normals[outsideIdx]) > 0 || 
                fabs(smallestInsideDist - smallestOutsideDist) <= COLLISION_DISTANCE_EPSILON) {

                DO_ASSIGNMENT(outsideIdx);
            }
            else {

                if (smallestInsideDist < smallestOutsideDist) {

                    // One final check: prioritize normal areas...
                    Collision::AABB2D insideNormalArea(this->lines[insideIdx].P1(), this->lines[insideIdx].P1());
                    insideNormalArea.AddPoint(this->lines[insideIdx].P2());
                    insideNormalArea.AddPoint(this->lines[insideIdx].P1() + 2*circle.Radius()*this->normals[insideIdx]);
                    insideNormalArea.AddPoint(this->lines[insideIdx].P2() + 2*circle.Radius()*this->normals[insideIdx]);

                    if (Collision::SqDistFromPtToAABB(insideNormalArea, cPointOfCollision) < EPSILON) {
                        DO_ASSIGNMENT(insideIdx);
                    }
                    else {
                        DO_ASSIGNMENT(outsideIdx);
                    }

                }
                else {
                    DO_ASSIGNMENT(outsideIdx);
                }
            }
        }

#undef DO_ASSIGNMENT
    }

    if (n.IsZero()) {
        return false;
    }

    return true;
}

// Ball-BoundingLines collisions where both the ball and these bounding lines are moving...
bool BoundingLines::Collide(double dT, const Collision::Circle2D& circle, const Vector2D& velocity, Vector2D& n, 
                            Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& cPointOfCollision, 
                            const Vector2D& lineVelocity) const {


    assert(circle.Radius() > 0);

    bool zeroLineVelocity = (lineVelocity == Vector2D(0.0f, 0.0f));
    if (zeroLineVelocity) {
        // Perform the simpler solution (where the line isn't moving)
        return this->Collide(dT, circle, velocity, n, collisionLine, timeUntilCollision, cPointOfCollision);
    }

    
    // Treat the problem like the line is stationary and the frame of reference is shifted to the circle
    Vector2D relativeVel = velocity - lineVelocity;
    if (!this->Collide(dT, circle, relativeVel, n, collisionLine, timeUntilCollision, cPointOfCollision)) {
        return false;
    }

    Vector2D relativeMoveVec = dT * relativeVel;
    float relativeMoveVecDist = relativeMoveVec.Magnitude();

    if (relativeMoveVecDist < EPSILON) {
        cPointOfCollision  = circle.Center();
        timeUntilCollision = 0.0;
        return true;
    }

    float collisionDist = (cPointOfCollision - circle.Center()).Magnitude();
    float fract = collisionDist / relativeMoveVecDist;
    if (fract > 1.0f) {
        return false;
    }
    
    // Remap back to the original frame of reference
    cPointOfCollision  = circle.Center() + fract * dT * velocity;
    timeUntilCollision = NumberFuncs::SignOf(fract) * (cPointOfCollision - circle.Center()).Magnitude() / velocity.Magnitude();
    return true;
}

/**
 * Calculate the closest point out of all the bounding lines in this object
 * to the given point.
 * Returns: Closest point on any of the bounding lines in this to pt.
 */
Point2D BoundingLines::ClosestPoint(const Point2D& pt) const {
	if (this->lines.size() == 0) {
		return pt;
	}

	Point2D closestPt = Collision::ClosestPoint(pt, this->lines[0]);
	float closestSqDist = Point2D::SqDistance(closestPt, pt);

	for (size_t i = 1; i < this->lines.size(); i++) {
		Point2D tempClosestPt		= Collision::ClosestPoint(pt, this->lines[i]);
		float tempClosestSqDist	= Point2D::SqDistance(tempClosestPt, pt);
		if (tempClosestSqDist < closestSqDist) {
			closestSqDist = tempClosestSqDist;
			closestPt = tempClosestPt;
		}
	}

	return closestPt;
}

/**
 * Determine whether the given point is inside this set of bounding lines or not.
 */
bool BoundingLines::IsInside(const Point2D& pt) const {
	// Go through each line and do a half plane test using that line on the point
	// based on the line's normal
	for (size_t i = 0; i < this->lines.size(); i++) {
		Vector2D fromLineToPt = pt - this->lines[i].P1();
		if (fromLineToPt == Vector2D(0, 0)) {
			fromLineToPt = pt - this->lines[i].P2();
			assert(fromLineToPt != Vector2D(0, 0));
		}

		Vector2D normal = this->normals[i];
		// If the dot product is negative then the point is inside...
		float dotResult = Vector2D::Dot(fromLineToPt, normal);
		if (dotResult >= 0.0f) {
			return false;
		}
	}

	return true;
}

/**
 * Check to see if the given circle collides with any of the bounding lines of this.
 */
bool BoundingLines::CollisionCheck(const Collision::Circle2D& c) const {
	const float sqrRadius = c.Radius() * c.Radius();
	for (size_t i = 0; i < this->lines.size(); i++) {
		Point2D closestPt = Collision::ClosestPoint(c.Center(), this->lines[i]);
		if (Point2D::SqDistance(closestPt, c.Center()) <= sqrRadius) {
			return true;
		}
	}

	return false;
}

bool BoundingLines::CollisionCheck(const Collision::AABB2D& aabb) const {
	for (size_t i = 0; i < this->lines.size(); ++i) {
		if (Collision::IsCollision(this->lines[i], aabb)) {
			return true;
		}
	}

	return false;
}



bool BoundingLines::CollisionCheck(const BoundingLines& other, double dT, const Vector2D& velocity) const {
    static const float SAMPLE_DISTANCE = LevelPiece::PIECE_HEIGHT / 2.0f;

    int numCollisionSamples;
    bool zeroVelocity = (velocity == Vector2D(0.0f, 0.0f));

    if (zeroVelocity) {
        numCollisionSamples = 1;
    }
    else {
        // Calculate the number of samples required to make sure that the increment distance is reasonable
        numCollisionSamples = static_cast<int>(ceilf(velocity.Magnitude() * dT / SAMPLE_DISTANCE));
        numCollisionSamples = std::min<int>(40, std::max<int>(1, numCollisionSamples+1));
    }

    // Figure out the distance along the vector traveled since the last collision to take each sample at...
    Vector2D sampleIncDist = dT * velocity / static_cast<float>(numCollisionSamples);
    BoundingLines sampleBounds = other;

    for (int i = 0; i < numCollisionSamples; i++) {
        if (this->CollisionCheck(sampleBounds)) {
            return true;
        }
        sampleBounds.TranslateBounds(sampleIncDist);
    }

    return false;
}

bool BoundingLines::CollisionCheck(const Collision::LineSeg2D& lineSeg) const {
	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];
		if (Collision::IsCollision(currThisLine, lineSeg)) {
			return true;
		}
	}

	return false;
}

/**
 * Test for a collision between this and the given set of lines and get the index of the first
 * line in this that collided.
 * Returns: the first index of the line in this that collided with the lines in other.
 * Returns -1 if no collision occurred.
 */
int BoundingLines::CollisionCheckIndex(const BoundingLines& other) const {
	int count = 0;

    Collision::AABB2D currLineAABB;
    Collision::AABB2D otherLineAABB;

	// Do a line-line collision with every line in this verses every line in the given set of BoundingLines
	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];
        currThisLine.BuildAABB(currLineAABB);

		for (size_t j = 0; j < other.lines.size(); ++j) {
			const Collision::LineSeg2D& currOtherLine = other.lines[j];
            currOtherLine.BuildAABB(otherLineAABB);
            
            // First do a test of the bounding boxes for the two lines
            if (Collision::IsCollision(currLineAABB, otherLineAABB)) {
                const Vector2D& currOtherNormal = other.normals[j];

                // Now do a more thorough test
                float t = Vector2D::Dot(currOtherNormal, (currOtherLine.P1() - currThisLine.P1())) /
                    Vector2D::Dot(currOtherNormal, (currThisLine.P2() - currThisLine.P1()));
                if (t >= 0.0f && t <= 1.0f) {
                    return count;
                }
            }
		}

		count++;
	}

	return -1;
}

/**
 * Obtain all of the line indices that are colliding with other, in this.
 * Returns: a set of all line indices being collided with in this object.
 */
std::vector<int> BoundingLines::CollisionCheckIndices(const BoundingLines& other) const {
	std::vector<int> indicesCollidedWith;
    indicesCollidedWith.reserve(this->lines.size());

	// Do a line-line collision with every line in this verses every line in the given set of BoundingLines
	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];

		for (size_t j = 0; j < other.lines.size(); ++j) {
			const Collision::LineSeg2D& currOtherLine = other.lines[j];

			if (Collision::IsCollision(currThisLine, currOtherLine)) {
				indicesCollidedWith.push_back(i);
                break;
			}
		}
	}

	return indicesCollidedWith;
}

std::vector<int> BoundingLines::CollisionCheckIndices(const Collision::Ray2D& ray) const {
    std::vector<int> indicesCollidedWith;
    indicesCollidedWith.reserve(this->lines.size());
    float temp;

    // Do a line-line collision with every line in this verses every line in the given set of BoundingLines
    for (size_t i = 0; i < this->lines.size(); ++i) {
        const Collision::LineSeg2D& currThisLine = this->lines[i];

        if (Collision::IsCollision(ray, currThisLine, temp)) {
            indicesCollidedWith.push_back(i);
            break;
        }
    }

    return indicesCollidedWith;
}

/**
 * Obtain all of the line indices that are colliding with the given lineSegment and this.
 * Returns: a set of all line indices being collided with in this object.
 */
std::vector<int> BoundingLines::CollisionCheckIndices(const Collision::LineSeg2D& lineSeg) const {
	std::vector<int> indicesCollidedWith;
	int count = 0;

	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];
		
		if (Collision::IsCollision(currThisLine, lineSeg)) {
			indicesCollidedWith.push_back(count);
		}
		
		count++;
	}

	return indicesCollidedWith;
}

/**
 * Get the closest line segment indices to the given point.
 * It is very unlikely that more than one index will be returned, in the case
 * of multiple indices they are all very close (not perfectly) to the same distance
 * from the given point.
 */
std::vector<int> BoundingLines::ClosestCollisionIndices(const Point2D& pt, float tolerance) const {
	int count = 0;
	float closestSqDist = FLT_MAX;
	std::vector<int> closestIndices;

	for (size_t i = 0; i < this->lines.size(); ++i) {
		// Check the square distance between the point and the current line segment...
		float currSqDist = Collision::SqDistFromPtToLineSeg(this->lines[i], pt);

		// Update the smallest distance line index - if there are multiple then add them to the array...
		if (currSqDist < closestSqDist) {
			closestIndices.clear();
			closestIndices.push_back(count);
			closestSqDist = currSqDist;
		}
		else if (fabs(currSqDist - closestSqDist) < tolerance) {
			closestIndices.push_back(count);
		}

		count++;
	}

	return closestIndices;
}

/**
 * Get a list of all collision points where lines in this are colliding with 
 * the lines in other.
 * Returns: true if there were collisions, false otherwise.
 */
bool BoundingLines::GetCollisionPoints(const BoundingLines& other, std::list<Point2D>& collisionPts) const {
	bool isCollision;
	Point2D collisionPt;
	for (size_t i = 0; i < this->lines.size(); ++i) {
		for (size_t j = 0; j < other.lines.size(); ++j) {
			isCollision = Collision::GetCollisionPoint(this->GetLine(i), other.GetLine(j), collisionPt);
			if (isCollision) {
				collisionPts.push_back(collisionPt);
			}
		}
	}
	
	return !(collisionPts.empty());
}

/**
 * Get a list of all the collision points where lines in this are colliding with the
 * given circle.
 * Returns: true if there were collisions, false otherwise.
 */
bool BoundingLines::GetCollisionPoints(const Collision::Circle2D& circle, std::list<Point2D>& collisionPts) const {
	bool isCollision;
	Point2D collisionPt;
	for (size_t i = 0; i < this->lines.size(); ++i) {
		isCollision = Collision::GetCollisionPoint(circle, this->GetLine(i), collisionPt);
		if (isCollision) {
			collisionPts.push_back(collisionPt);
		}
	}
	
	return !(collisionPts.empty());
}

bool BoundingLines::GetCollisionPoints(const Collision::AABB2D& aabb, std::list<Point2D>& collisionPts) const {
	bool isCollision;
	Point2D collisionPt;
	for (size_t i = 0; i < this->lines.size(); ++i) {
		isCollision = Collision::GetCollisionPoint(aabb, this->GetLine(i), collisionPt);
		if (isCollision) {
			collisionPts.push_back(collisionPt);
		}
	}
	
	return !(collisionPts.empty());
}

/**
 * Check to see whether this collided with a ray. Finds the closest collision among
 * all the lines in these bounding lines and returns the parameter required to get the point
 * of the collision by plugging it into the ray equation.
 * Returns: true if any lines in this collided with the given ray, false otherwise.
 */
bool BoundingLines::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	rayT = FLT_MAX;
	bool collisionFound = false;

	for (size_t i = 0; i < this->lines.size(); ++i) {
		float currentT;
		if (Collision::IsCollision(ray, this->lines[i], currentT)) {
			collisionFound = true;
			if (currentT < rayT) {
				rayT = currentT;
			}
		}
	}

	return collisionFound;
}

/**
 * Rotates the bounding lines of this by the given angle in degrees.
 */
void BoundingLines::RotateLinesAndNormals(float angleInDegs, const Point2D& rotationCenter) {
	
	// Rotate each line...
	for (std::vector<Collision::LineSeg2D>::iterator iter = this->lines.begin(); iter != this->lines.end(); ++iter) {
		Collision::LineSeg2D& currLineSeg = *iter;
		currLineSeg.Rotate(angleInDegs, rotationCenter);
	}

	// Rotate each normal...
	for (std::vector<Vector2D>::iterator iter = this->normals.begin(); iter != this->normals.end(); ++iter) {
		Vector2D& currNormal = *iter;
		currNormal = Vector2D::Rotate(angleInDegs, currNormal);
	}
}

/**
 * Translates the bounding lines by the given vector.
 */
void BoundingLines::TranslateBounds(const Vector2D& translation) {
	// Translate each line...
	for (std::vector<Collision::LineSeg2D>::iterator iter = this->lines.begin(); iter != this->lines.end(); ++iter) {
		Collision::LineSeg2D& currLineSeg = *iter;
		currLineSeg.Translate(translation);
	}
}

void BoundingLines::ReflectX() {
    for (std::vector<Collision::LineSeg2D>::iterator iter = this->lines.begin(); iter != this->lines.end(); ++iter) {
        Collision::LineSeg2D& currLineSeg = *iter;
        currLineSeg.ReflectX();
    }
    for (std::vector<Vector2D>::iterator iter = this->normals.begin(); iter != this->normals.end(); ++iter) {
        Vector2D& currNormal = *iter;
        currNormal[0] *= -1.0f;
    }
}

void BoundingLines::Transform(const Matrix4x4& transform) {

    // Transform each line...
	for (std::vector<Collision::LineSeg2D>::iterator iter = this->lines.begin(); iter != this->lines.end(); ++iter) {
		Collision::LineSeg2D& currLineSeg = *iter;
		currLineSeg.Transform(transform);
	}

    // Transform each normal...
	for (std::vector<Vector2D>::iterator iter = this->normals.begin(); iter != this->normals.end(); ++iter) {
		Vector2D& currNormal = *iter;
        Vector3D temp = transform * Vector3D(currNormal[0], currNormal[1], 0.0f);
        currNormal = Vector2D(temp[0], temp[1]);
        currNormal.Normalize();
	}
}

void BoundingLines::SetAllBoundsInside(bool inside) {
    for (std::vector<bool>::iterator iter = this->onInside.begin(); iter != this->onInside.end(); ++iter) {
        *iter = inside;
    }
}

void BoundingLines::SetBoundInside(int index, bool inside) {
    assert(index < static_cast<int>(this->onInside.size()) && index >= 0);
    this->onInside[index] = inside;
}

BoundingLines& BoundingLines::operator=(const BoundingLines& copy) {
    if (this == &copy) {
        return (*this);
    }

    this->lines = copy.lines;
    this->normals = copy.normals;
    this->onInside = copy.onInside;

    return (*this);
}

void BoundingLines::DrawSimpleBounds() const {
    glBegin(GL_LINES);
	for (int i = 0; i < static_cast<int>(this->lines.size()); i++) {
        const Collision::LineSeg2D& currLine = this->lines[i];
        glVertex2f(currLine.P1()[0], currLine.P1()[1]);
        glVertex2f(currLine.P2()[0], currLine.P2()[1]);
    }
    glEnd();
}

void BoundingLines::DebugDraw() const {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw bounding lines
	glLineWidth(1.0f);
	
    glBegin(GL_LINES);
	for (int i = 0; i < static_cast<int>(this->lines.size()); i++) {
        if (this->onInside[i]) {
            glColor3f(0,1,1);
        }
        else {
            glColor3f(1,0,1);
        }
		const Collision::LineSeg2D& currLine = this->lines[i];
		glVertex2f(currLine.P1()[0], currLine.P1()[1]);
		glVertex2f(currLine.P2()[0], currLine.P2()[1]);
	}
	glEnd();

	// Draw normals
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < this->normals.size(); i++) {
		Vector2D currNorm = 0.5f*this->normals[i];
		Collision::LineSeg2D currLine = this->lines[i];
		
		// Place normal at midpoint
		Vector2D midPt = 0.5f * (Vector2D(currLine.P1()[0], currLine.P1()[1]) + Vector2D(currLine.P2()[0], currLine.P2()[1]));

		glVertex2f(midPt[0], midPt[1]);
		glVertex2f(midPt[0] + currNorm[0], midPt[1] + currNorm[1]);
	}
	glEnd();

	glPopAttrib();
}