#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "BasicIncludes.h"
#include "Algebra.h"

class Matrix4x4;

// 2D Vector -----------------------------------------------------
class Vector2D {
private:
  float v_[2];

public:
  Vector2D() {
    v_[0] = 0.0f;
    v_[1] = 0.0f;
  }

  Vector2D(float x, float y) { 
    v_[0] = x;
    v_[1] = y;
  }

  Vector2D(const Vector2D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
  }

  Vector2D& operator =(const Vector2D& other) {
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

	static float Magnitude(const Vector2D& v1) {
		return sqrtf(v1[0]*v1[0] + v1[1]*v1[1]);
	}

	static float Dot(const Vector2D& v1, const Vector2D& v2) {
		return v1[0]*v2[0] + v1[1]*v2[1];
	}

	static Vector2D Normalize(const Vector2D& v) {
		Vector2D temp(v);
		temp.Normalize();
		return temp;
	}

	void Normalize() {
		float magnitude = Vector2D::Magnitude(*this);
		assert(magnitude != 0);
		this->v_[0] /= magnitude;
		this->v_[1] /= magnitude;
	}

};

inline std::ostream& operator <<(std::ostream& os, const Vector2D& v) {
  return os << "v<" << v[0] << "," << v[1] << ">";
}

inline Vector2D operator *(float s, const Vector2D& v)
{
  return Vector2D(s*v[0], s*v[1]);
}

inline Vector2D operator +(const Vector2D& a, const Vector2D& b) {
  return Vector2D(a[0]+b[0], a[1]+b[1]);
}

inline Vector2D operator -(const Vector2D& a, const Vector2D& b) {
  return Vector2D(a[0]-b[0], a[1]-b[1]);
}

inline Vector2D operator -(const Vector2D& a) {
  return Vector2D(-a[0], -a[1]);
}

inline bool operator ==(const Vector2D& a, const Vector2D& b) {
	return (abs(a[0] - b[0]) < EPSILON) && (abs(a[1] - b[1]) < EPSILON);
}

inline bool operator !=(const Vector2D& a, const Vector2D& b) {
	return !(a==b);
}

inline Vector2D Reflect(const Vector2D& v, const Vector2D& n) {
	Vector2D newVel = v - 2.0f * Vector2D::Dot(n, v) * n;
	return newVel;
}

inline Vector2D Rotate(float angInDegs, const Vector2D& v) {
	float angInRads = Trig::degreesToRadians(angInDegs);
	return Vector2D(cosf(angInRads) * v[0] - sinf(angInRads) * v[1], sinf(angInRads) * v[0] + cosf(angInRads) * v[1]);
}

// 3D Vector -----------------------------------------------------
class Vector3D {
private:
  float v_[3];

public:
  Vector3D() {
    v_[0] = 0.0f;
    v_[1] = 0.0f;
    v_[2] = 0.0f;
  }

  Vector3D(float x, float y, float z) { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }

  Vector3D(const Vector3D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  }

  Vector3D& operator =(const Vector3D& other) {
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
	bool operator==(const Vector3D &other) {
		return 	fabs(v_[0] - other[0]) < EPSILON &&
		  			fabs(v_[1] - other[1]) < EPSILON &&
		  			fabs(v_[2] - other[2]) < EPSILON;
	}

  float dot(const Vector3D& other) const {
    return v_[0]*other.v_[0] + v_[1]*other.v_[1] + v_[2]*other.v_[2];
  }	
  float length2() const {
    return v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2];
  }

  float length() const {
		return static_cast<float>(sqrt(length2()));
  }

  
	void Normalize() {
		float magnitude = this->length();
		assert(magnitude != 0);
		this->v_[0] /= magnitude;
		this->v_[1] /= magnitude;
		this->v_[2] /= magnitude;
	}

  Vector3D cross(const Vector3D& other) const {
    return Vector3D(v_[1]*other[2] - v_[2]*other[1],
                    v_[2]*other[0] - v_[0]*other[2],
                    v_[0]*other[1] - v_[1]*other[0]);
  }

  static Vector3D cross(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D(v1[1] * v2[2] - v1[2] * v2[1],
                    v1[2] * v2[0] - v1[0] * v2[2],
                    v1[0] * v2[1] - v1[1] * v2[0]);
  }

	static Vector3D Normalize(const Vector3D& v) {
		Vector3D temp(v);
		temp.Normalize();
		return temp;
	}

	/**
	 * Finds the angle between two given vectors, in radians.
	 * Precondition: v1 and v2 MUST be normalized for this to work!!
	 * Returns: angle between v1 and v2 in radians.
	 */
	static float AngleBetweenInRadians(const Vector3D& v1, const Vector3D& v2) {
		return acosf(v1.dot(v2));
	}

	/**
	 * To create an 'imperfect' perpendicular vector to the one given we
	 * find the smallest index coordinate component and set it to zero,
	 * then we flip the other two coordinates and negate the first.
	 * Returns: An 'imperfect' perpendicular vector to the given v.
	 **/
	static Vector3D MollerHughesPerpendicular(const Vector3D& v) {
		Vector3D perpendicularMH = v;
		int smallestIndex;
		
		// First we take the smallest coordinate component and set it to zero
		if (v[0] < v[1]) {
			if (v[0] < v[2]) {
				smallestIndex = 0;
				perpendicularMH[1] = -v[2];
				perpendicularMH[2] =  v[1]; 
			}
			else {
				smallestIndex = 2;
				perpendicularMH[0] = -v[1];
				perpendicularMH[1] =  v[0]; 
			}
		}
		else {
			if (v[1] < v[2]) {
				smallestIndex = 1;
				perpendicularMH[0] = -v[2];
				perpendicularMH[2] =  v[0]; 
			}
			else {
				smallestIndex = 2;
				perpendicularMH[0] = -v[1];
				perpendicularMH[1] =  v[0]; 
			}
		}

		perpendicularMH[smallestIndex] = 0;
		return perpendicularMH;
	}


	// Return spherical coords (r, theta (the angle to z-axis), phi (the angle to x-axis))
	static Vector3D ToSphericalFromCartesian(const Vector3D& v) {
		Vector3D sphericalCoords;
		sphericalCoords[0] = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		sphericalCoords[1] = acosf(v[2] / sphericalCoords[0]);
		sphericalCoords[2] = atan2f(v[1], v[0]);
		return sphericalCoords;
	}

	static Vector3D ToCartesianFromSpherical(const Vector3D& v) {
		Vector3D cartesianCoords;
		float rTimesSinTheta = v[2] * sinf(v[1]);
		cartesianCoords[0] = rTimesSinTheta * cosf(v[2]);
		cartesianCoords[1] = rTimesSinTheta * sinf(v[2]);
		cartesianCoords[2] = v[2] * cosf(v[1]);
		return cartesianCoords;
	}
};

inline std::ostream& operator <<(std::ostream& os, const Vector3D& v) {
  return os << "v<" << v[0] << "," << v[1] << "," << v[2] << ">";
}

inline Vector3D operator *(float s, const Vector3D& v)
{
  return Vector3D(s*v[0], s*v[1], s*v[2]);
}
inline Vector3D operator /(const Vector3D& v, float d) {
	assert(d != 0);
	return Vector3D(v[0]/d, v[1]/d, v[2]/d);
}

inline Vector3D operator +(const Vector3D& a, const Vector3D& b) {
  return Vector3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Vector3D operator -(const Vector3D& a, const Vector3D& b) {
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a) {
  return Vector3D(-a[0], -a[1], -a[2]);
}

inline Vector3D cross(const Vector3D& a, const Vector3D& b) {
  return a.cross(b);
}

inline bool operator ==(const Vector3D& a, const Vector3D& b) {
	return (abs(a[0] - b[0]) < EPSILON) && (abs(a[1] - b[1]) < EPSILON) && (abs(a[2] - b[2]) < EPSILON);
}

inline bool operator !=(const Vector3D& a, const Vector3D& b) {
	return !(a==b);
}

Vector3D operator *(const Matrix4x4& M, const Vector3D& v);
Vector3D transNorm(const Matrix4x4& M, const Vector3D& n);


// 4D Vector -----------------------------------------------------
class Vector4D {

private:
  float v_[4];

public:
  Vector4D() {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
    v_[3] = 0.0;
  } 
	Vector4D(float x, float y, float z, float w) { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
    v_[3] = w;
  }

  Vector4D(const Vector4D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
  }

  Vector4D& operator =(const Vector4D& other) {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
    return *this;
  }

  float& operator[](size_t idx) {
		assert(idx < 4);
    return v_[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 4);
    return v_[ idx ];
  }

	static float Dot(const Vector4D& v1, const Vector4D& v2) {
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
	}

};

inline std::ostream& operator <<(std::ostream& os, const Vector4D& v) {
  return os << "v<" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ">";
}

#endif