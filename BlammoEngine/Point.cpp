/**
 * Point.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Point.h"
#include "Matrix.h"

// 3D operations

Point3D operator +(const Point3D& a, const Vector3D& b) {
  return Point3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

Point3D operator -(const Point3D& a, const Vector3D& b) {
  return Point3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

Vector3D operator -(const Point3D& a, const Point3D& b) {
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}