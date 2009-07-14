#ifndef __POINT_H__
#define __POINT_H__

#include "BasicIncludes.h"
#include "Algebra.h"

//class Vector2D;
//class Vector3D;
class Matrix4x4;

#include "Vector.h"

// Simple 2D Point --------------------------------
class Point2D {
private:
  float v_[2];

public:
  Point2D() {
    v_[0] = 0.0;
    v_[1] = 0.0;
  }

  Point2D(float x, float y) { 
    v_[0] = x;
    v_[1] = y;
  }

  Point2D(const Point2D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
  }

  Point2D& operator =(const Point2D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    return *this;
  }

  float& operator[](size_t idx) {
		assert(idx < 2);
    return v_[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 2);
    return v_[ idx ];
  }
  
  // Robust equivalence
	bool operator==(const Point2D &other) {
		return 	fabs(v_[0] - other[0]) < EPSILON &&
		  			fabs(v_[1] - other[1]) < EPSILON;
	}

	static Point2D GetMidPoint(const Point2D& p1, const Point2D& p2) {
		return Point2D((p1[0] + p2[0]) / 2.0f, (p1[1] + p2[1]) / 2.0f);
	}
};

// 2D Point output function for debugging purposes
inline std::ostream& operator <<(std::ostream& os, const Point2D& p)
{
  return os << "p<" << p[0] << "," << p[1] << ">";
}


inline Point2D operator +(const Point2D& a, const Vector2D& b) {
  return Point2D(a[0]+b[0], a[1]+b[1]);
}
inline Vector2D operator -(const Point2D& a, const Point2D& b) {
  return Vector2D(a[0]-b[0], a[1]-b[1]);
}
inline Point2D operator -(const Point2D& a, const Vector2D& b) {
  return Point2D(a[0]-b[0], a[1]-b[1]);
}

// Simple 3D Point --------------------------------
class Point3D {

private:
  float v_[3];

public:
  Point3D() {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
  }

  Point3D(float x, float y, float z) { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }

  Point3D(const Point3D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  }

  Point3D& operator =(const Point3D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    return *this;
  }

  float& operator[](size_t idx) {
		assert(idx < 3);
    return v_[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 3);
    return v_[ idx ];
  }

	
	// Robust equivalence
	bool operator==(const Point3D &other) {
		return 	fabs(v_[0] - other[0]) < EPSILON &&
		  			fabs(v_[1] - other[1]) < EPSILON &&
		  			fabs(v_[2] - other[2]) < EPSILON;
	}

};

inline std::ostream& operator <<(std::ostream& os, const Point3D& p) {
  return os << "p<" << p[0] << "," << p[1] << "," << p[2] << ">";
}

inline Point3D operator *(const float f, const Point3D& p) {
	return Point3D(f*p[0], f*p[1], f*p[2]);
}

inline Point3D operator /(const Point3D& p, const float f) {
	return Point3D(p[0]/f, p[1]/f, p[2]/f);
}

Point3D operator *(const Matrix4x4& M, const Point3D& p);
Point3D operator +(const Point3D& a, const Vector3D& b);
Vector3D operator -(const Point3D& a, const Point3D& b);
Point3D operator -(const Point3D& a, const Vector3D& b);

#endif