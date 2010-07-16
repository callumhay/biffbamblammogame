#ifndef __PLANE_H__
#define __PLANE_H__

#include "Vector.h"

/**
 * Defines a 2D plane in 3D space.
 */
class Plane {
public:
	Plane(const Vector3D& unitNormal, const Point3D& ptOnPlane) : normal(unitNormal), point(ptOnPlane) {}

	const Vector3D& GetUnitNormal() const { return this->normal; }
	const Point3D& GetPointOnPlane() const { return this->point; }

private:
	Vector3D normal;	// Normal the is perpendicular to the plane on the positive side
	Point3D point;		// Point on the plane

};

#endif // __PLANE_H__