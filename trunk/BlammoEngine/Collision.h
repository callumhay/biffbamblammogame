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

	/**
	 * 2D Axis-Aligned Bounding Box for collision detection.
	 */
	class AABB2D {
	private:
		Point2D minCoord, maxCoord;
	public:
		AABB2D(const Point2D &min, const Point2D &max): minCoord(min), maxCoord(max) {}
		~AABB2D() {}

		Point2D GetMin() const {
			return this->minCoord;
		}
		Point2D GetMax() const {
			return this->maxCoord;
		}
	};

	class LineSeg2D {

	private:
		Point2D p1, p2;

	public:
		LineSeg2D() {};
		LineSeg2D(const Point2D& p1, const Point2D& p2) : p1(p1), p2(p2) {}
		virtual ~LineSeg2D() {}


		void SetP1(const Point2D& p) {
			this->p1 = p;
		}
		const Point2D& P1() const {
			return this->p1;
		}

		void SetP2(const Point2D& p) {
			this->p2 = p;
		}
		const Point2D& P2() const {
			return this->p2;
		}

		void Rotate(float angleInDegs, const Point2D& rotationCenter);
		void Translate(const Vector2D& translation);

	};

	inline void LineSeg2D::Rotate(float angleInDegs, const Point2D& rotationCenter) {
		Vector2D p1Vec = this->p1 - rotationCenter;
		Vector2D p2Vec = this->p2 - rotationCenter;
		p1Vec = Vector2D::Rotate(angleInDegs, p1Vec);
		p2Vec = Vector2D::Rotate(angleInDegs, p2Vec);
		this->SetP1(rotationCenter + p1Vec);
		this->SetP2(rotationCenter + p2Vec);
	}

	inline void LineSeg2D::Translate(const Vector2D& translation) {
		this->SetP1(this->p1 + translation);
		this->SetP2(this->p2 + translation);
	}

	class LineSeg3D {

	private:
		Point3D p1, p2;

	public:
		LineSeg3D() {};
		LineSeg3D(const Point3D& p1, const Point3D& p2) : p1(p1), p2(p2) {}
		~LineSeg3D() {}

		void SetP1(const Point3D& p) {
			this->p1 = p;
		}
		Point3D P1() const {
			return this->p1;
		}

		void SetP2(const Point3D& p) {
			this->p2 = p;
		}
		Point3D P2() const {
			return this->p2;
		}

		Vector3D GetUnitDirection() const {
			Vector3D direction = p2 - p1;
			direction.Normalize();
			return direction;
		}

	};

	class Ray2D {
	public:
		Ray2D() {};
		Ray2D(const Point2D& origin, const Vector2D& dir) : origin(origin), unitDirection(dir) {
			assert(Vector2D::Magnitude(dir) - 1.0f <= EPSILON);
		}
		~Ray2D() {};

		const Point2D& GetOrigin() const { return this->origin; }
		const Vector2D& GetUnitDirection() const { return this->unitDirection; }

		void SetOrigin(const Point2D& origin) { this->origin = origin; }
		void SetUnitDirection(const Vector2D& unitDir) { this->unitDirection = unitDir; }

		// Obtain a point along the ray a given distance (dist) from the ray's origin
		Point2D GetPointAlongRayFromOrigin(float dist) const {
			return this->origin + (dist * this->unitDirection);
		}

	private:
		Point2D origin;
		Vector2D unitDirection;
	};

	class Circle2D {
	private:
		Point2D center;
		float radius;

	public:
		Circle2D(const Point2D& c, float r): center(c), radius(r) {}
		~Circle2D(){}
		
		void SetCenter(const Point2D& c) {
			this->center = c;
		}
		const Point2D& Center() const {
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
	 * Helper function: computes two times the signed triangle area. The result is positive
	 * if abc is ccw, negative if abc is cw, zero if abc is degenerate.
	 */
	inline float Signed2DTriArea(const Point2D& a, const Point2D& b, const Point2D& c) {
		return (a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]);
	}

	/**
	 * Calculates and returns the square distance from a given point to a given line segment.
	 * This is for 2D lines and points.
	 * Returns: Square distance from point to lineSeg.
	 */
	inline float SqDistFromPtToLineSeg(const LineSeg2D& lineSeg, const Point2D& point) { 
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
	inline float SqDistFromPtToAABB(const AABB2D& aabb, const Point2D& pt) {
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
	inline bool IsCollision(const AABB2D &a, const AABB2D &b) {
		if (a.GetMax()[0] < b.GetMin()[0] || a.GetMin()[0] > b.GetMax()[0]) return false;
		if (a.GetMax()[1] < b.GetMin()[1] || a.GetMin()[1] > b.GetMax()[1]) return false;
		return true;
	}

	/**
	 * Test whether a circle and 2D aabb collide.
	 * Returns: true on collision, false otherwise.
	 */
	inline bool IsCollision(const AABB2D& aabb, const Circle2D& circle) {
		float sqrDist = Collision::SqDistFromPtToAABB(aabb, circle.Center());
		return sqrDist <= (circle.Radius() * circle.Radius());
	}


	/**
	 * Figures out if two 2D line segments collide or not.
	 * Returns: true on collision, false otherwise.
	 */
	inline bool IsCollision(const LineSeg2D& l1, const LineSeg2D& l2) {
		float a1 = Signed2DTriArea(l1.P1(), l1.P2(), l2.P2());
		float a2 = Signed2DTriArea(l1.P1(), l1.P2(), l2.P1());

		if (a1 * a2 < 0.0f) {
			float a3 = Signed2DTriArea(l2.P1(), l2.P2(), l1.P1());
			float a4 = a3 + a2 - a1;
			if (a3 * a4 < 0.0f) {
				return true;
			}
		}
		
		return false;
	}

	/**
	 * Check for collision between a 2D ray and AABB, calculate the ray parameter for distance
	 * multiplier from its origin.
	 */
	inline bool IsCollision(const Ray2D& ray, const AABB2D& aabb, float& tMin) {
		const Point2D& RAY_ORIGIN = ray.GetOrigin();
		const Vector2D& RAY_DIR   = ray.GetUnitDirection();
		
		const Point2D& AABB_MIN   = aabb.GetMin();
		const Point2D& AABB_MAX	 = aabb.GetMax();

		tMin = 0.0f;
		float tMax = FLT_MAX;

		for (int i = 0; i < 2; i++) {

			if (fabs(RAY_DIR[i]) < EPSILON) {
				if (RAY_ORIGIN[i] < AABB_MIN[i] || RAY_ORIGIN[i] > AABB_MAX[i]) {
					return false;
				}
			}
			else {
				float ood = 1.0f / RAY_DIR[i];
				float t1  = (AABB_MIN[i] - RAY_ORIGIN[i]) * ood;
				float t2  = (AABB_MAX[i] - RAY_ORIGIN[i]) * ood;
				if (t1 > t2) {
					std::swap(t1, t2);
				}
				
				tMin = std::max<float>(tMin, t1);
				tMax = std::min<float>(tMax, t2);
				if (tMin > tMax) {
					return false;
				}
			}
		}
		
		return true;
	}

	inline bool IsCollision(const Ray2D& ray, const LineSeg2D& lineSeg, float& rayT) {
		// Create a parameteric equation for the line segment
		Vector2D D1 = lineSeg.P2() - lineSeg.P1();
		Vector2D D0  = ray.GetUnitDirection();
			
		Vector2D perpD1(D1[1], -D1[0]);
		float dotPerpD1D0 = Vector2D::Dot(perpD1, D0);
		if (fabs(dotPerpD1D0) < EPSILON) {
			// Ray and line segment are parallel...
			return false;
		}

		Vector2D perpD0(D0[1], -D0[0]);
		Vector2D P1MinusP0 = lineSeg.P1() - ray.GetOrigin();
		//P1MinusP0.Normalize();
		rayT = Vector2D::Dot(perpD1, P1MinusP0) / dotPerpD1D0;
		float lineT = Vector2D::Dot(perpD0, P1MinusP0) / dotPerpD1D0;

		return (rayT >= 0 && lineT >= 0 && lineT <= 1);
	}

	inline bool IsCollision(const Ray2D& ray, const Circle2D& circle, float& rayT) {
		Vector2D m = ray.GetOrigin() - circle.Center();
		float b    = Vector2D::Dot(m, ray.GetUnitDirection());
		float c    = Vector2D::Dot(m, m) - (circle.Radius() * circle.Radius());

		if (c > 0.0f && b > 0.0f) {
			return false;
		}

		float discr = b*b - c;
		if (discr < 0.0f) {
			return false;
		}

		rayT = -b - sqrt(discr);
		if (rayT < 0.0f) {
			rayT = 0.0f;
		}
		return true;
	}

	// Closest Point Computations ******************************************************
	
	/**
	 * Calculate the closest point on the given line segment to the given point.
	 * Returns: The closest point on lineSeg to pt.
	 */
	inline Point2D ClosestPoint(const Point2D& pt, const LineSeg2D& lineSeg) {
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
	inline Point3D ClosestPoint(const Point3D& pt, const LineSeg3D& lineSeg) {
		Vector3D lineSegDir = lineSeg.P2() - lineSeg.P1();
		float t = Vector3D::Dot(pt - lineSeg.P1(), lineSegDir) / Vector3D::Dot(lineSegDir, lineSegDir);

		if (t < 0.0f) {
			t = 0.0f;
		}
		else if (t > 1.0f) {
			t = 1.0f;
		}

		return lineSeg.P1() + t * lineSegDir;
	}

	/**
	 * Calculate the closest points for two line segments.
	 * Returns: The shortest square distance between two line segments and
	 * the closest points on each corresponding segment.
	 */
	inline float ClosestPoint(const LineSeg2D& lineSeg1, const LineSeg2D& lineSeg2, 
		Point2D& closestPtOnLineSeg1, Point2D& closestPtOnLineSeg2) {
		
			float s, t;

			Vector2D d1 = lineSeg1.P2() - lineSeg1.P1();
			Vector2D d2 = lineSeg2.P2() - lineSeg2.P1();
			Vector2D r  = lineSeg1.P1() - lineSeg2.P1();

			float a = Vector2D::Dot(d1, d1);
			float e = Vector2D::Dot(d2, d2);
			float f = Vector2D::Dot(d2, r);

			if (a <= EPSILON && e <= EPSILON) {
				closestPtOnLineSeg1 = lineSeg1.P1();
				closestPtOnLineSeg2 = lineSeg2.P1();
				return Vector2D::Dot(closestPtOnLineSeg1 - closestPtOnLineSeg2, closestPtOnLineSeg1 - closestPtOnLineSeg2);
			}

			if (a <= EPSILON) {
				s = 0.0f;
				t = NumberFuncs::Clamp(f / e, 0.0f, 1.0f);
			}
			else {
				float c = Vector2D::Dot(d1, r);
				if (e <= EPSILON) {
					t = 0.0f;
					s = NumberFuncs::Clamp(-c / a, 0.0f, 1.0f);
				}
				else {
					float b = Vector2D::Dot(d1, d2);	
					float denom = a*e - b*b;

					if (denom != 0.0f) {
						s = NumberFuncs::Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
					}
					else {
						s = 0.0f;
					}
					
					t = (b*s + f) / e;
					if (t < 0.0f) {
						t = 0.0f;
						s = NumberFuncs::Clamp(-c / a, 0.0f, 1.0f);
					}
					else if (t > 1.0f) {
						t = 1.0f;
						s = NumberFuncs::Clamp((b - c) / a, 0.0f, 1.0f);
					}
				}
			}

			closestPtOnLineSeg1 = lineSeg1.P1() + s * d1;
			closestPtOnLineSeg2 = lineSeg2.P1() + t * d2;
			return Vector2D::Dot(closestPtOnLineSeg1 - closestPtOnLineSeg2, closestPtOnLineSeg1 - closestPtOnLineSeg2);
	}

};

#endif