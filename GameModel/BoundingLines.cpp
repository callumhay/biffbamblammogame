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
bool BoundingLines::Collide(const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, float &d) {
	float sqRadius = c.Radius()*c.Radius();
	d = 0.0f;

	// If there's no velocity then just exit with no collision (how can a non-moving object collide?)
	if (velocity == Vector2D(0.0f, 0.0f)) {
		return false;
	}

	Vector2D normalizedVel = Vector2D::Normalize(velocity);
	float minSqDist = FLT_MAX;	// Use this to ensure that we bounce off the most relevant side

	std::vector<Vector2D> collisionNormals;
	float largestDistanceMag = FLT_MIN;
	Vector2D largestDistMagNormal;

	// For each of the lines check for collision and how close the collision is to the line
	for (size_t i = 0; i < this->lines.size(); i++) {
		
		// Collision candiates must be within a 90 degree angle of the normal for the line
		// being collided with (otherwise they're mistakingly colliding with another line)
		bool isCollisionCandidate = acos(Vector2D::Dot(-normalizedVel, this->normals[i])) < M_PI_DIV2;
		if (isCollisionCandidate) {
			float sqDist = Collision::SqDistFromPtToLineSeg(this->lines[i], c.Center());
		
			// Check to see if there was a collision between the current line seg and the given circle
			if (sqDist < sqRadius) { //&& sqDist < minSqDist) {

				// Collision occurred set the normal
				collisionNormals.push_back(this->normals[i]);
				
				float currDist = sqrtf(sqDist);
				if (Vector2D::Dot(this->normals[i], c.Center() - this->lines[i].P1()) < 0) {
					currDist *= -1.0f; 
				}
				
				if (fabs(currDist) > fabs(largestDistanceMag)) {
					largestDistanceMag = currDist;
					largestDistMagNormal = this->normals[i];
				}
			}
		}
	}

	bool collisionOccurred = collisionNormals.size() > 0;
	if (collisionOccurred) {
		// There was a collision - find a weighted average of the normals and distances...
		for (size_t i = 0; i < collisionNormals.size(); i++) {
			// Check to see if the normal is ridiculously different...
			if (acos(Vector2D::Dot(largestDistMagNormal, collisionNormals[i])) <= M_PI_DIV2) {
				// Normal is within a reaonable range of the closest line collision's normal, so use it
				// in the averaging process
				n = n + collisionNormals[i];
			}
		}

		n.Normalize();
		d = largestDistanceMag;
	}

	return collisionOccurred;
}

/**
 * Calculate the closest point out of all the bounding lines in this object
 * to the given point.
 * Returns: Closest point on any of the bounding lines in this to pt.
 */
Point2D BoundingLines::ClosestPoint(const Point2D& pt) {
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

void BoundingLines::DebugDraw() const {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

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
	glEnable(GL_LIGHTING);
}