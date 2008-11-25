#include "BoundingLines.h"

#include "../BlammoEngine/BlammoEngine.h"

BoundingLines::BoundingLines(const std::vector<LineSeg2D>& lines, const std::vector<Vector2D>& norms) : 
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
bool BoundingLines::Collide(const Circle2D& c, Vector2D& n, float &d) {
	std::vector<Vector2D> collisionNorms;
	float sqRadius = c.Radius()*c.Radius();
	d = 0.0f;

	float minSqDist = FLT_MAX;	// Use this to ensure that we bounce off the most relevant side

	// For each of the lines check for collision and how close the collision is to the line
	for (size_t i = 0; i < this->lines.size(); i++) {
		float sqDist = Collision::SqDistFromPtToLineSeg(this->lines[i], c.Center());
	
		// Check to see if there was a collision with the current line seg and circle
		// and that the collision is the most relevant
		if (sqDist < sqRadius && sqDist <= (minSqDist + EPSILON)) {
			
			// Collision occurred add the normal to the list
			collisionNorms.push_back(this->normals[i]);
			minSqDist = sqDist;

			d = sqrtf(sqDist);
			if (Vector2D::Dot(this->normals[i], c.Center() - this->lines[i].P1()) < 0) {
				d *= -1.0f; 
			}
		}
	}

	if (collisionNorms.size() == 0) {
		return false;
	}

	// Average the normals
	n = Vector2D(0,0);
	for (size_t i = 0; i < collisionNorms.size(); i++) {
		n = n + collisionNorms[i];
	}
	if (collisionNorms.size() > 1) {
		n.Normalize();
	}

	return true;
}

void BoundingLines::DebugDraw() const {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// Draw bounding lines
	glLineWidth(1.0f);
	glColor3f(1,0,1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < this->lines.size(); i++) {
		LineSeg2D currLine = this->lines[i];
		glVertex2f(currLine.P1()[0], currLine.P1()[1]);
		glVertex2f(currLine.P2()[0], currLine.P2()[1]);
	}
	glEnd();

	// Draw normals
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < this->normals.size(); i++) {
		Vector2D currNorm = this->normals[i];
		LineSeg2D currLine = this->lines[i];
		
		// Place normal at midpoint
		Vector2D midPt = 0.5f * (Vector2D(currLine.P1()[0], currLine.P1()[1]) + Vector2D(currLine.P2()[0], currLine.P2()[1]));

		glVertex2f(midPt[0], midPt[1]);
		glVertex2f(midPt[0] + currNorm[0], midPt[1] + currNorm[1]);
	}
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}