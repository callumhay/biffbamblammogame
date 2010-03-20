/**
 * BoundingLines.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BoundingLines.h"

BoundingLines::BoundingLines(const std::vector<Collision::LineSeg2D>& lines, const std::vector<Vector2D>& norms) : 
	lines(lines), normals(norms) {
	assert(lines.size() == norms.size());
}

BoundingLines::~BoundingLines() {
}

/**
 * Test collision between the bounding lines of this and the given circle c.
 * Return: The average normal for the lines that the given circle collided with in the value n. 
 * Also returns the distance from the center of the circle to the line (negative d means the circle is
 * inside the line, postivie means outside - the outward normal pointing direction).
 * The boolean return value will be true if one or more collisions occured, false otherwise.
 */
bool BoundingLines::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
														Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	n = Vector2D(0, 0);
	timeSinceCollision = -1.0;

	// If there's no velocity then just exit with no collision (how can a non-moving object collide?)
	if (velocity == Vector2D(0.0f, 0.0f)) {
		return false;
	}

	float velocityMagnitude = Vector2D::Magnitude(velocity);
	Vector2D normalizedVel = velocity / velocityMagnitude;

	// Use a square radius instead of using square root operations (it's faster.)
	float sqRadius = c.Radius()*c.Radius();
	bool collisionOccurred = false;
	float smallestCollisionDistSq = FLT_MAX;
	float movementDistOverDT = dT * velocityMagnitude;
	Vector2D movementOverDT = dT * velocity;
	Point2D previousCenter = c.Center() - movementOverDT;
	Point2D closestCapsulePt, closestLinePt;
	float collisionParamT;

	// Create lines for the center outer sides of the capsule and collide them with lines of this bounding line set
	Collision::LineSeg2D centerRange(previousCenter, c.Center());
	Collision::LineSeg2D extendedCenterRange(previousCenter - c.Radius()*normalizedVel, c.Center() + c.Radius()*normalizedVel);

	Collision::Ray2D capsuleSide1Ray(previousCenter + c.Radius() * Vector2D(normalizedVel[1], -normalizedVel[0]), normalizedVel);
	Collision::Ray2D capsuleSide2Ray(previousCenter - c.Radius() * Vector2D(normalizedVel[1], -normalizedVel[0]), normalizedVel);

	int count = 0;
	for (std::vector<Collision::LineSeg2D>::const_iterator lineIter = this->lines.begin(); lineIter != this->lines.end(); ++lineIter) {
		const Collision::LineSeg2D& currLine = *lineIter;
		const Vector2D& currNormal = this->normals[count];

		// Cast a ray from the previous extent of circle towards the colliding line
		Collision::Ray2D circleCollisionRay(previousCenter, normalizedVel);
		// Check to see if the ray collides with the current line segment
		
		if (Collision::IsCollision(circleCollisionRay, currLine, collisionParamT)) {
			// NOTE: collisionParamT is gaurenteed to be positive here
			assert(collisionParamT >= 0.0);

			// Figure out what the value of the ray parameter was at the time of collision - this value might be positive or
			// negative depending on how far/close the ball was from the collision line
			double rayTCenterDuringCollision = collisionParamT - c.Radius();
			// Now check to see if there was a collision within the last time interval dT
			if (rayTCenterDuringCollision <= movementDistOverDT) {
				Point2D centerOfCircleAtCollision = circleCollisionRay.GetPointAlongRayFromOrigin(rayTCenterDuringCollision);
				
				// Now figure out the distance between the current center circle position and the
				// position of the center at collision
				float distance = Point2D::Distance(c.Center(), centerOfCircleAtCollision);
				float timeToCollision = distance / velocityMagnitude;
				assert(timeToCollision >= 0.0);

				if (timeToCollision > timeSinceCollision) {
					timeSinceCollision = timeToCollision;
					collisionOccurred = true;
					n = n + currNormal;
					collisionLine = currLine;
				}
			}			
		}

		if (Collision::IsCollision(capsuleSide1Ray, currLine, collisionParamT)) {
			if (collisionParamT <= movementDistOverDT) {
				float distance = movementDistOverDT - collisionParamT;
				float timeToCollision = distance / velocityMagnitude;
				assert(timeToCollision >= 0.0);

				if (timeToCollision > timeSinceCollision) {
					timeSinceCollision = timeToCollision;
					collisionOccurred = true;
					n = n + currNormal;
					collisionLine = currLine;
				}
			}
		}
		if (Collision::IsCollision(capsuleSide2Ray, currLine, collisionParamT)) {
			if (collisionParamT <= movementDistOverDT) {
				float distance = movementDistOverDT - collisionParamT;
				float timeToCollision = distance / velocityMagnitude;
				assert(timeToCollision >= 0.0);

				if (timeToCollision > timeSinceCollision) {
					timeSinceCollision = timeToCollision;
					collisionOccurred = true;
					n = n + currNormal;
					collisionLine = currLine;
				}
			}
		}

		if (!collisionOccurred) {
			float sqDist = Collision::SqDistFromPtToLineSeg(currLine, c.Center());
			if (sqDist <= sqRadius) {

				// Check to see if it's still colliding at the start of the time delta...
				sqDist = Collision::SqDistFromPtToLineSeg(currLine, previousCenter);
				if (sqDist <= sqRadius) {
					// Not so good... seems that it's still colliding at the start of the time segment,
					// return that this is so
					timeSinceCollision = dT;
					collisionOccurred = true;
					n = n + currNormal;
					collisionLine = currLine;
					break;
				}

				// Binary search for a point when the collision was close enough
				Point2D currSearchCenter = previousCenter;
				float searchFraction = 1.0f;
				int count = 0;
				while (fabs(sqDist - sqRadius) > 0.001 || count > 100) {
					searchFraction *= 0.5f;
					if (sqDist > sqRadius) {
						currSearchCenter = currSearchCenter + searchFraction * movementDistOverDT * normalizedVel;
					}
					else {
						currSearchCenter = currSearchCenter - searchFraction * movementDistOverDT * normalizedVel;
					}
					sqDist = Collision::SqDistFromPtToLineSeg(currLine, currSearchCenter);
					count++;
				}

				// Now figure out the distance between the current center circle position and the
				// position of the center at collision
				float distance = Point2D::Distance(c.Center(), currSearchCenter);
				float timeToCollision = distance / velocityMagnitude;
				assert(timeToCollision >= 0.0);

				if (timeToCollision > timeSinceCollision) {
					timeSinceCollision = timeToCollision;
					collisionOccurred = true;
					n = n + currNormal;
					collisionLine = currLine;
				}
			}
		}

		count++;
	}

	if (collisionOccurred) {
		// Average of all the normals collided with
		n.Normalize();
	}

	return collisionOccurred;
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
 * Check to see whether this collided with another set of bounding lines.
 * Returns: true if any lines in this collided with any lines in the given BoundingLines object,
 * false otherwise.
 */
bool BoundingLines::CollisionCheck(const BoundingLines& other) const {
	return this->CollisionCheckIndex(other) != -1;
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
	for (std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin(); thisIter != this->lines.end(); ++thisIter) {
		const Collision::LineSeg2D& currThisLine = *thisIter;

		for (std::vector<Collision::LineSeg2D>::const_iterator otherIter = other.lines.begin(); otherIter != other.lines.end(); ++otherIter) {
			const Collision::LineSeg2D& currOtherLine = *otherIter;

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
	for (std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin(); thisIter != this->lines.end(); ++thisIter) {
		const Collision::LineSeg2D& currThisLine = *thisIter;

		for (std::vector<Collision::LineSeg2D>::const_iterator otherIter = other.lines.begin(); otherIter != other.lines.end(); ++otherIter) {
			const Collision::LineSeg2D& currOtherLine = *otherIter;

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

	for (std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin(); thisIter != this->lines.end(); ++thisIter) {
		const Collision::LineSeg2D& currThisLine = *thisIter;
		
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

	for (std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin(); thisIter != this->lines.end(); ++thisIter) {
		// Check the square distance between the point and the current line segment...
		float currSqDist = Collision::SqDistFromPtToLineSeg(*thisIter, pt);

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
 * Check to see whether this collided with a ray. Finds the closest collision among
 * all the lines in these bounding lines and returns the parameter required to get the point
 * of the collision by plugging it into the ray equation.
 * Returns: true if any lines in this collided with the given ray, false otherwise.
 */
bool BoundingLines::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	rayT = FLT_MAX;
	bool collisionFound = false;

	for (std::vector<Collision::LineSeg2D>::const_iterator thisIter = this->lines.begin(); thisIter != this->lines.end(); ++thisIter) {
		float currentT;
		if (Collision::IsCollision(ray, *thisIter, currentT)) {
			collisionFound = true;
			if (currentT < rayT) {
				rayT = currentT;
			}
		}
	}

	return collisionFound;
}

void BoundingLines::DebugDraw() const {
	glDisable(GL_DEPTH_TEST);

	// Draw bounding lines
	glLineWidth(1.0f);
	glColor3f(1,0,1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < this->lines.size(); i++) {
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

	glEnable(GL_DEPTH_TEST);
}