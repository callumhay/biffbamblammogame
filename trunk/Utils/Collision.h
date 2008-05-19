#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "Vector.h"
#include "Point.h"
#include "Shape2D.h"

class Collision {

private:
	Collision(){}

public:
	~Collision(){}
	/**
	 * Calculates and returns the square distance from a given point to a given line segment.
	 * This is for 2D lines and points.
	 */
	static float Collision::SqDistFromPtToLineSeg(const LineSeg2D& lineSeg, const Point2D& point) { 
		Vector2D ab = lineSeg.P2() - lineSeg.P1();
		Vector2D ac = point - lineSeg.P1();
		Vector2D bc = point - lineSeg.P2();

		// Cases where c projects outside of ab
		float e = Vector2D::Dot(ac, ab);
		if (e <= EPSILON) { 
			return Vector2D::Dot(ac, ac);
		}

		float f = Vector2D::Dot(ab, ab);
		if (e >= f) {
			return Vector2D::Dot(bc, bc);
		}
		
		// Case where c projects onto ab
		return Vector2D::Dot(ac, ac) - (e * e / f);
	}
	
};

#endif