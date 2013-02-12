/**
 * BoundingLines.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BoundingLines.h"

BoundingLines::BoundingLines(const std::vector<Collision::LineSeg2D>& lines,
                             const std::vector<Vector2D>& norms) : 
	lines(lines), normals(norms) {
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
 * Test collision between the bounding lines of this and the given circle c.
 * Return: The average normal for the lines that the given circle collided with in the value n. 
 * Also returns the distance from the center of the circle to the line (negative d means the circle is
 * inside the line, postivie means outside - the outward normal pointing direction).
 * The boolean return value will be true if one or more collisions occured, false otherwise.
 */
bool BoundingLines::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
							Collision::LineSeg2D& collisionLine, int& collisionLineIdx, double& timeSinceCollision) const {


    static const int NUM_COLLISON_SAMPLES = 16;

    bool zeroVelocity = (velocity == Vector2D(0.0f, 0.0f));
    int numCollisionSamples;
    if (zeroVelocity) {
        numCollisionSamples = 1;
    }
    else {
        numCollisionSamples = NUM_COLLISON_SAMPLES;
    }
    
    // Figure out the distance along the vector travelled since the last collision
    // to take each sample at...
    Vector2D sampleIncDist = dT * velocity / static_cast<float>(numCollisionSamples);
    double   sampleIncTime = dT / static_cast<double>(numCollisionSamples);

    Point2D currSamplePt = c.Center();
    double currTimeSinceCollision = 0.0;

    // Keep track of all the indices collided with and the collision point collided at
    std::vector<size_t> collisionLineIdxs;
    collisionLineIdxs.reserve(this->GetNumLines());
    Point2D collisionPt;
    bool isCollision = false;

    // Go through all of the samples starting with the first (which is just a bit off from the previous
    // tick location) and moving towards the circle's current location, when a collision is found we exit
    for (int i = 0; i < numCollisionSamples; i++) {
        for (int lineIdx = 0; lineIdx < static_cast<int>(this->lines.size()); ++lineIdx) {
            bool tempIsCollision = Collision::GetCollisionPoint(Collision::Circle2D(currSamplePt, c.Radius()), 
                                                                this->lines[lineIdx], collisionPt);
            if (tempIsCollision) {
                collisionLineIdxs.push_back(lineIdx);
                isCollision = true;
            }
        }
        if (isCollision) { break; }
        currSamplePt = currSamplePt + sampleIncDist;
        currTimeSinceCollision += sampleIncTime;
    }

    if (!isCollision) {
        return false;
    }

    assert(!collisionLineIdxs.empty());
    timeSinceCollision = currTimeSinceCollision;

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

        for (int i = 0; i < static_cast<int>(collisionLineIdxs.size()); i++) {
            
            int lineIdx = collisionLineIdxs[i];
            const Collision::LineSeg2D& currLine = this->lines[lineIdx];
            const Vector2D& currNormal = this->normals[lineIdx];

            float d    = Vector2D::Dot(currNormal, Vector2D(currLine.P1()[0], currLine.P1()[1]));
            float dist = Vector2D::Dot(currNormal, Vector2D(currSamplePt[0], currSamplePt[1])) - d;
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
            const float COLLISION_DISTANCE_EPSILON = c.Radius() / 50.0f;

            // There is an outside and an inside found in the tie... if they are really close together
            // then we favour the outside boundary over the inside...
            if (fabs(smallestInsideDist - smallestOutsideDist) <= COLLISION_DISTANCE_EPSILON) {
                DO_ASSIGNMENT(outsideIdx);
            }
            else {
                
                if (smallestInsideDist < smallestOutsideDist) {

                    // One final check: prioritize normal areas...
                    Collision::AABB2D insideNormalArea(this->lines[insideIdx].P1(), this->lines[insideIdx].P1());
                    insideNormalArea.AddPoint(this->lines[insideIdx].P2());
                    insideNormalArea.AddPoint(this->lines[insideIdx].P1() + 2*c.Radius()*this->normals[insideIdx]);
                    insideNormalArea.AddPoint(this->lines[insideIdx].P2() + 2*c.Radius()*this->normals[insideIdx]);

                    if (Collision::SqDistFromPtToAABB(insideNormalArea, currSamplePt) < EPSILON) {
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

    //n.Normalize();

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

/**
 * Check to see whether this collided with another set of bounding lines.
 * Returns: true if any lines in this collided with any lines in the given BoundingLines object,
 * false otherwise.
 */
bool BoundingLines::CollisionCheck(const BoundingLines& other) const {
	return this->CollisionCheckIndex(other) != -1;
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

	// Do a line-line collision with every line in this verses every line in the given set of BoundingLines
	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];

		for (size_t j = 0; j < other.lines.size(); ++j) {
			const Collision::LineSeg2D& currOtherLine = other.lines[j];

			if (Collision::IsCollision(currThisLine, currOtherLine)) {
				return count;
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
	std::set<int> indicesCollidedWith;
	int count = 0;

	// Do a line-line collision with every line in this verses every line in the given set of BoundingLines
	for (size_t i = 0; i < this->lines.size(); ++i) {
		const Collision::LineSeg2D& currThisLine = this->lines[i];

		for (size_t j = 0; j < other.lines.size(); ++j) {
			const Collision::LineSeg2D& currOtherLine = other.lines[j];

			if (Collision::IsCollision(currThisLine, currOtherLine)) {
				indicesCollidedWith.insert(count);
			}
		}

		count++;
	}

	std::vector<int> tempVecOfIndices;
	tempVecOfIndices.reserve(indicesCollidedWith.size());
	for (std::set<int>::iterator iter = indicesCollidedWith.begin(); iter != indicesCollidedWith.end(); ++iter) {
		tempVecOfIndices.push_back(*iter);
	}

	return tempVecOfIndices;
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

BoundingLines& BoundingLines::operator=(const BoundingLines& copy) {
    if (this == &copy) {
        return (*this);
    }

    this->lines = copy.lines;
    this->normals = copy.normals;
    this->onInside = copy.onInside;

    return (*this);
}

void BoundingLines::DebugDraw() const {
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw bounding lines
	glLineWidth(1.0f);
	
    glBegin(GL_LINES);
	for (size_t i = 0; i < this->lines.size(); i++) {
        if (this->onInside[i]) {
            glColor3f(0,1,1);
        }
        else {
            glColor3f(1,0,1);
        }
		Collision::LineSeg2D currLine = this->lines[i];
		glVertex2f(currLine.P1()[0], currLine.P1()[1]);
		glVertex2f(currLine.P2()[0], currLine.P2()[1]);
	}
	glEnd();

	// Draw normals
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < this->normals.size(); i++) {
		Vector2D currNorm = this->normals[i];
		Collision::LineSeg2D currLine = this->lines[i];
		
		// Place normal at midpoint
		Vector2D midPt = 0.5f * (Vector2D(currLine.P1()[0], currLine.P1()[1]) + Vector2D(currLine.P2()[0], currLine.P2()[1]));

		glVertex2f(midPt[0], midPt[1]);
		glVertex2f(midPt[0] + currNorm[0], midPt[1] + currNorm[1]);
	}
	glEnd();

	glPopAttrib();
}