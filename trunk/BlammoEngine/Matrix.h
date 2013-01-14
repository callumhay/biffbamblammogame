/**
 * Matrix.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "BasicIncludes.h"
#include "Algebra.h"
#include "Vector.h"
#include "Point.h"

class Matrix4x4 {

private:
  float v_[16];

public:
  Matrix4x4() {
    // Construct an identity matrix
    std::fill(v_, v_+16, 0.0f);
    v_[0] = 1.0;
    v_[5] = 1.0;
    v_[10] = 1.0;
    v_[15] = 1.0;
  }

	Matrix4x4(const Matrix4x4& other) {
    std::copy(other.v_, other.v_+16, v_);
  }

	Matrix4x4(const float values[16]) {
		for (int i = 0; i < 16; i++) {
			v_[i] = values[i];
		}
	}

  Matrix4x4(const Vector4D& row1, const Vector4D& row2, const Vector4D& row3, const Vector4D& row4) {
    v_[0] = row1[0]; 
    v_[4] = row1[1]; 
    v_[8] = row1[2]; 
    v_[12] = row1[3]; 

    v_[1] = row2[0]; 
    v_[5] = row2[1]; 
    v_[9] = row2[2]; 
    v_[13] = row2[3]; 

    v_[2] = row3[0]; 
    v_[6] = row3[1]; 
    v_[10] = row3[2]; 
    v_[14] = row3[3]; 

    v_[3] = row4[0]; 
    v_[7] = row4[1]; 
    v_[11] = row4[2]; 
    v_[15] = row4[3]; 
  }

	Matrix4x4(const Vector3D& row1, const Vector3D& row2, const Vector3D& row3) {
    v_[0]  = row1[0]; 
    v_[4]  = row1[1]; 
    v_[8]  = row1[2]; 
    v_[12] = 0; 

    v_[1]  = row2[0]; 
    v_[5]  = row2[1]; 
    v_[9]  = row2[2]; 
    v_[13] = 0; 

    v_[2]  = row3[0]; 
    v_[6]  = row3[1]; 
    v_[10] = row3[2]; 
    v_[14] = 0; 

    v_[3]  = 0; 
    v_[7]  = 0; 
    v_[11] = 0; 
    v_[15] = 1; 
	}

  Matrix4x4& operator=(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
    return *this;
  }

  Vector4D getRow(size_t row) const {
		assert(row >= 0 && row <= 3);
    return Vector4D(v_[row], v_[row + 4], v_[row + 8], v_[row + 12]);
  }

  Vector4D getColumn(size_t col) const {
		assert(col >= 0 && col <= 3);
    return Vector4D(v_[4*col], v_[1+4*col], v_[2+4*col], v_[3+4*col]);
  }

	Point3D getTranslationPt3D() const {
		return Point3D(v_[12], v_[13], v_[14]);
	}
    Point2D getTranslationPt2D() const {
        return Point2D(v_[12], v_[13]);
    }
    Vector3D getTranslationVec3D() const {
        return Vector3D(v_[12], v_[13], v_[14]);
    }
    Vector2D getTranslationVec2D() const {
        return Vector2D(v_[12], v_[13]);
    }
	void setTranslation(const Point3D& t) {
		v_[12] = t[0];
		v_[13] = t[1]; 
		v_[14] = t[2];
	}

  float& operator[](size_t idx) {
		assert(idx < 16);
    return v_[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 16);
    return v_[ idx ];
  }

  Matrix4x4 transpose() const {
    return Matrix4x4(getColumn(0), getColumn(1), 
                      getColumn(2), getColumn(3));
  }

	Matrix4x4 inverse() const;

  const float* begin() const {
    return v_;
  }

  const float *end() const {
    return begin() + 16;
  }

	static Matrix4x4 translationMatrix(const Vector3D &translation) {
		// Pretty easy, just put the displacement as the last column in the matrix
		Matrix4x4 t(Vector4D(1, 0, 0, translation[0]),
					      Vector4D(0, 1, 0, translation[1]),
					      Vector4D(0, 0, 1, translation[2]),
					      Vector4D(0, 0, 0, 1));

		return t;	
	}

	static Matrix4x4 rotationMatrix(char axis, float angle, bool inDegrees = true) {
		float radAngle = angle;
		if (inDegrees){
  		radAngle = Trig::degreesToRadians(angle);
		}
		
		Vector4D row1 = Vector4D(1, 0, 0, 0);
		Vector4D row2 = Vector4D(0, 1, 0, 0);
		Vector4D row3 = Vector4D(0, 0, 1, 0);
		Vector4D row4 = Vector4D(0, 0, 0, 1);

		switch (axis){
		case 'x':
			row2 = Vector4D(0, cos(radAngle), -sin(radAngle), 0);
			row3 = Vector4D(0, sin(radAngle), cos(radAngle), 0);
			break;
		case 'y':
			row1 = Vector4D(cos(radAngle), 0, sin(radAngle), 0);
			row3 = Vector4D(-sin(radAngle), 0, cos(radAngle), 0);
			break;
		case 'z':
			row1 = Vector4D(cos(radAngle), -sin(radAngle), 0, 0);
			row2 = Vector4D(sin(radAngle), cos(radAngle), 0, 0);
			break;
		default:
			// Just return an identity matrix
			assert(false);
			break;
		}

		// Create and return the matrix based on the values calculated
		Matrix4x4 r(row1, row2, row3, row4);
		return r;
	}

	static Matrix4x4 rotationZMatrix(float angleInDegs) {
		float radAngle = Trig::degreesToRadians(angleInDegs);
		Vector4D row1 = Vector4D(cos(radAngle), -sin(radAngle), 0, 0);
		Vector4D row2 = Vector4D(sin(radAngle), cos(radAngle), 0, 0);
		Vector4D row3 = Vector4D(0, 0, 1, 0);
		Vector4D row4 = Vector4D(0, 0, 0, 1);

		// Create and return the matrix based on the values calculated
		Matrix4x4 r(row1, row2, row3, row4);
		return r;
	}

	static Matrix4x4 rotationMatrix(float angleInRads, const Vector3D& rotationVec) {
		Vector3D unitRotVec = Vector3D::Normalize(rotationVec);
		if (unitRotVec == Vector3D(0,0,0)) {
			assert(false);
			return Matrix4x4();
		}

		float c = cos(angleInRads);
		float s = sin(angleInRads);
		float oneMinusC = (1.0f - c);
		float uX = unitRotVec[0];
		float uY = unitRotVec[1];
		float uZ = unitRotVec[2];
		float uXSq = uX * uX;
		float uYSq = uY * uY;
		float uZSq = uZ * uZ;

		Vector4D row1 = Vector4D(uXSq + (1 - uXSq)*c, uX*uY*oneMinusC - uZ*s, uX*uZ*oneMinusC + uY*s, 0);
		Vector4D row2 = Vector4D(uX*uY*oneMinusC + uZ*s, uYSq + (1 - uYSq)*c, uY*uZ*oneMinusC - uX*s, 0);
		Vector4D row3 = Vector4D(uX*uZ*oneMinusC - uY*s, uY*uZ*oneMinusC + uX*s, uZSq + (1 - uZSq)*c, 0);
		Vector4D row4 = Vector4D(0, 0, 0, 1);

		// Create and return the matrix based on the values calculated
		Matrix4x4 r(row1, row2, row3, row4);
		return r;
	}

	static Matrix4x4 reflectionMatrix(char axis) {
		Vector4D row1 = Vector4D(1, 0, 0, 0);
		Vector4D row2 = Vector4D(0, 1, 0, 0);
		Vector4D row3 = Vector4D(0, 0, 1, 0);
		Vector4D row4 = Vector4D(0, 0, 0, 1);

		switch (axis){
		case 'x':
			row2[1] = -1;
			row3[2] = -1;
			break;
		case 'y':
			row1[0] = -1;
			row3[2] = -1;
			break;
		case 'z':
			row1[0] = -1;
			row2[1] = -1;
			break;
		default:
			// Just return an identity matrix
			assert(false);
			break;
		}
				
		return Matrix4x4(row1, row2, row3, row4);
	}

	static Matrix4x4 scaleMatrix(const Vector3D &scale) {
		// Pretty easy, scales just make up the diagonal of the matrix
		Matrix4x4 s(Vector4D(scale[0], 0, 0, 0),
					Vector4D(0, scale[1], 0, 0),
					Vector4D(0, 0, scale[2], 0),
					Vector4D(0, 0, 0, 1));

		return s;
	}
};

inline Matrix4x4 operator *(const Matrix4x4& a, const Matrix4x4& b)
{
  Matrix4x4 ret;

  for (int i = 0; i < 4; ++i) {
    Vector4D lhsRow = a.getRow(i);
		
    for (int j = 0; j < 4; ++j) {
			int jTimes4 = j*4;
			Vector4D rhsCol(b[jTimes4], b[jTimes4+1], b[jTimes4+2], b[jTimes4+3]);
			ret[i + jTimes4] = Vector4D::Dot(lhsRow, rhsCol);
    }
  }

  return ret;
}

inline Vector3D operator*(const Matrix4x4& m, const Vector3D& v) {
	Vector3D ret;

	for (int i = 0; i < 3; i++) {

		Vector4D currRow = m.getRow(i);
		float sum = 0.0f;

		for (int j = 0; j < 3; j++) {
			sum += v[j] * currRow[j];
		}
		ret[i] = sum;
	}

	return ret;
}

inline Point3D operator*(const Matrix4x4& m, const Point3D& p) {
	Point3D ret;
	Vector4D temp = Vector4D(p[0], p[1], p[2], 1);
	
	for (int i = 0; i < 3; i++) {
		Vector4D currRow = m.getRow(i);
		float sum = 0.0f;

		for (int j = 0; j < 4; j++) {
			sum += temp[j] * currRow[j];
		}
		ret[i] = sum;
	}

	return ret;
}

inline Matrix4x4 Matrix4x4::inverse() const {
  float fA0 = v_[ 0]*v_[ 5] - v_[ 1]*v_[ 4];
  float fA1 = v_[ 0]*v_[ 6] - v_[ 2]*v_[ 4];
  float fA2 = v_[ 0]*v_[ 7] - v_[ 3]*v_[ 4];
  float fA3 = v_[ 1]*v_[ 6] - v_[ 2]*v_[ 5];
  float fA4 = v_[ 1]*v_[ 7] - v_[ 3]*v_[ 5];
  float fA5 = v_[ 2]*v_[ 7] - v_[ 3]*v_[ 6];
  float fB0 = v_[ 8]*v_[13] - v_[ 9]*v_[12];
  float fB1 = v_[ 8]*v_[14] - v_[10]*v_[12];
  float fB2 = v_[ 8]*v_[15] - v_[11]*v_[12];
  float fB3 = v_[ 9]*v_[14] - v_[10]*v_[13];
  float fB4 = v_[ 9]*v_[15] - v_[11]*v_[13];
  float fB5 = v_[10]*v_[15] - v_[11]*v_[14];

  float fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
  if (fabs(fDet) <= EPSILON) {
      return Matrix4x4(Vector3D(0,0,0), Vector3D(0,0,0), Vector3D(0,0,0));
  }

  Matrix4x4 kInv;
  kInv.v_[ 0] =  v_[ 5]*fB5 - v_[ 6]*fB4 + v_[ 7]*fB3;
  kInv.v_[ 4] = -v_[ 4]*fB5 + v_[ 6]*fB2 - v_[ 7]*fB1;
  kInv.v_[ 8] =  v_[ 4]*fB4 - v_[ 5]*fB2 + v_[ 7]*fB0;
  kInv.v_[12] = -v_[ 4]*fB3 + v_[ 5]*fB1 - v_[ 6]*fB0;
  kInv.v_[ 1] = -v_[ 1]*fB5 + v_[ 2]*fB4 - v_[ 3]*fB3;
  kInv.v_[ 5] =  v_[ 0]*fB5 - v_[ 2]*fB2 + v_[ 3]*fB1;
  kInv.v_[ 9] = -v_[ 0]*fB4 + v_[ 1]*fB2 - v_[ 3]*fB0;
  kInv.v_[13] =  v_[ 0]*fB3 - v_[ 1]*fB1 + v_[ 2]*fB0;
  kInv.v_[ 2] =  v_[13]*fA5 - v_[14]*fA4 + v_[15]*fA3;
  kInv.v_[ 6] = -v_[12]*fA5 + v_[14]*fA2 - v_[15]*fA1;
  kInv.v_[10] =  v_[12]*fA4 - v_[13]*fA2 + v_[15]*fA0;
  kInv.v_[14] = -v_[12]*fA3 + v_[13]*fA1 - v_[14]*fA0;
  kInv.v_[ 3] = -v_[ 9]*fA5 + v_[10]*fA4 - v_[11]*fA3;
  kInv.v_[ 7] =  v_[ 8]*fA5 - v_[10]*fA2 + v_[11]*fA1;
  kInv.v_[11] = -v_[ 8]*fA4 + v_[ 9]*fA2 - v_[11]*fA0;
  kInv.v_[15] =  v_[ 8]*fA3 - v_[ 9]*fA1 + v_[10]*fA0;

  float fInvDet = ((float)1.0)/fDet;
  kInv.v_[ 0] *= fInvDet;
  kInv.v_[ 1] *= fInvDet;
  kInv.v_[ 2] *= fInvDet;
  kInv.v_[ 3] *= fInvDet;
  kInv.v_[ 4] *= fInvDet;
  kInv.v_[ 5] *= fInvDet;
  kInv.v_[ 6] *= fInvDet;
  kInv.v_[ 7] *= fInvDet;
  kInv.v_[ 8] *= fInvDet;
  kInv.v_[ 9] *= fInvDet;
  kInv.v_[10] *= fInvDet;
  kInv.v_[11] *= fInvDet;
  kInv.v_[12] *= fInvDet;
  kInv.v_[13] *= fInvDet;
  kInv.v_[14] *= fInvDet;
  kInv.v_[15] *= fInvDet;

  return kInv;
}

#endif