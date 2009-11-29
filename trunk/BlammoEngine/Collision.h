/**
 * Collision.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "Vector.h"
#include "Point.h"

namespace Collision {

	class Collidable {
	public:
		virtual ~Collidable(){};
	};

	/**
	 * 2D Axis-Aligned Bounding Box for collision detection.
	 */
	class AABB2D : public Collidable {
	private:
		Point2D minCoord, maxCoord;
	public:
		AABB2D(const Point2D &min, const Point2D &max): Collidable(), minCoord(min), maxCoord(max) {}
		virtual ~AABB2D() {}

		Point2D GetMin() const {
			return this->minCoord;
		}
		Point2D GetMax() const {
			return this->maxCoord;
		}
	};

	class LineSeg2D : public Collidable {

	private:
		Point2D p1, p2;

	public:
		LineSeg2D() {};
		LineSeg2D(const Point2D& p1, const Point2D& p2) : Collidable(), p1(p1), p2(p2) {}
		virtual ~LineSeg2D() {}


		void SetP1(const Point2D& p) {
			this->p1 = p;
		}
		Point2D P1() const {
			return this->p1;
		}

		void SetP2(const Point2D& p) {
			this->p2 = p;
		}
		Point2D P2() const {
			return this->p2;
		}

	};

	class Circle2D : public Collidable {
	private:
		Point2D center;
		float radius;

	public:
		Circle2D(const Point2D& c, float r): Collidable(), center(c), radius(r) {}
		virtual ~Circle2D(){}
		
		void SetCenter(const Point2D& c) {
			this->center = c;
		}
		Point2D Center() const {
			return this->center;
		}
		void SetRadius(float r) {
			this->radius = r;
		}
		float Radius() const {
			return this->radius;
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
	 * Calculates and returns the square distance from a given point to an axis-aligned
	 * bounding box, both in 2D.
	 * Returns: Square distance from aabb to pt.
	 */
	static float SqDistFromPtToAABB(const AABB2D& aabb, const Point2D& pt) {
		float sqDist = 0.0f;
		
		for (int i = 0; i < 2; i++) {
			float v = pt[i];
			if (v < aabb.GetMin()[i]) { 
				sqDist += (aabb.GetMin()[i] - v) * (aabb.GetMin()[i] - v);
			}
			if (v > aabb.GetMax()[i]) {
				sqDist += (v - aabb.GetMax()[i]) * (v - aabb.GetMax()[i]);
			}
		}
		return sqDist;
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

	// Closest Point Computations ******************************************************
	
	/**
	 * Calculate the closest point on the given line segment to the given point.
	 * Returns: The closest point on lineSeg to pt.
	 */
	static Point2D ClosestPoint(const Point2D& pt, const LineSeg2D& lineSeg) {
		Vector2D lineSegDir = lineSeg.P2() - lineSeg.P1();
		float t = Vector2D::Dot(pt - lineSeg.P1(), lineSegDir) / Vector2D::Dot(lineSegDir, lineSegDir);

		if (t < 0.0f) {
			t = 0.0f;
		}
		else if (t > 1.0f) {
			t = 1.0f;
		}

		return lineSeg.P1() + t * lineSegDir;
	}

};

#endif