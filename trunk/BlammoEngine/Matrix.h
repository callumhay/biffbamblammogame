#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "BasicIncludes.h"
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

	Point3D getTranslation() const {
		return Point3D(v_[12], v_[13], v_[14]);
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

  const float *begin() const {
    return (float*)v_;
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

inline Vector3D operator *(const Matrix4x4& m, const Vector3D& v) {
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

inline Point3D operator *(const Matrix4x4& m, const Point3D& p) {
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

#endif