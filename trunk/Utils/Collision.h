#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "Vector.h"
#include "Point.h"
#include "Shape2D.h"

namespace Collision {

	/**
	 * 2D Axis-Aligned Bounding Box for collision detection.
	 */
	class AABB2D {
	private:
		Point2D minCoord, maxCoord;
	public:
		AABB2D(const Point2D &min, const Point2D &max): minCoord(min), maxCoord(max) {}
		Point2D GetMin() const {
			return this->minCoord;
		}
		Point2D GetMax() const {
			return this->maxCoord;
		}
	};

	/**
	 * Calculates and returns the square distance from a given point to a given line segment.
	 * This is for 2D lines and points.
	 * Returns: Square distance from point to lineSeg.
	 */
	static float SqDistFromPtToLineSeg(const LineSeg2D& lineSeg, const Point2D& point) { 
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

	/**
	 * Figures out whether two 2D AABBs collide or not.
	 * Returns: true on collision, false otherwise.
	 */
	static bool IsCollision(const AABB2D &a, const AABB2D &b) {
		if (a.GetMax()[0] < b.GetMin()[0] || a.GetMin()[0] > b.GetMax()[0]) return false;
		if (a.GetMax()[1] < b.GetMin()[1] || a.GetMin()[1] > b.GetMax()[1]) return false;
		return true;
	}

};

#endif