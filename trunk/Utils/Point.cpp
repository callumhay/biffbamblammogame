#include "Point.h"
#include "Matrix.h"

// 3D operations

Point3D operator +(const Point3D& a, const Vector3D& b) {
  return Point3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

Point3D operator -(const Point3D& a, const Vector3D& b) {
  return Point3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

Point3D operator *(const Matrix4x4& M, const Point3D& p) {
  return Point3D(p[0] * M[0][0] + p[1] * M[0][1] + p[2] * M[0][2] + M[0][3],
                 p[0] * M[1][0] + p[1] * M[1][1] + p[2] * M[1][2] + M[1][3],
                 p[0] * M[2][0] + p[1] * M[2][1] + p[2] * M[2][2] + M[2][3]);
}

Vector3D operator -(const Point3D& a, const Point3D& b) {
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}