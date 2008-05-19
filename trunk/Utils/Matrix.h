#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Vector.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <assert.h>

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
  Matrix4x4(const Vector4D row1, const Vector4D row2, const Vector4D row3, const Vector4D row4) {
    v_[0] = row1[0]; 
    v_[1] = row1[1]; 
    v_[2] = row1[2]; 
    v_[3] = row1[3]; 

    v_[4] = row2[0]; 
    v_[5] = row2[1]; 
    v_[6] = row2[2]; 
    v_[7] = row2[3]; 

    v_[8] = row3[0]; 
    v_[9] = row3[1]; 
    v_[10] = row3[2]; 
    v_[11] = row3[3]; 

    v_[12] = row4[0]; 
    v_[13] = row4[1]; 
    v_[14] = row4[2]; 
    v_[15] = row4[3]; 
  }

  Matrix4x4& operator=(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
    return *this;
  }

  Vector4D getRow(size_t row) const {
    return Vector4D(v_[4*row], v_[4*row+1], v_[4*row+2], v_[4*row+3]);
  }
  float *getRow(size_t row) 
  {
    return (float*)v_ + 4*row;
  }

  Vector4D getColumn(size_t col) const {
    return Vector4D(v_[col], v_[4+col], v_[8+col], v_[12+col]);
  }

  Vector4D operator[](size_t row) const {
    return getRow(row);
  }
  float *operator[](size_t row) {
    return getRow(row);
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
	
	Matrix4x4 invert() const;
		
	static Matrix4x4 translationMatrix(const Vector3D &translation);
  static Matrix4x4 rotationMatrix(char axis, float angle, bool inDegrees = true);
  static Matrix4x4 scaleMatrix(const Vector3D &scale);	
};

inline Matrix4x4 operator *(const Matrix4x4& a, const Matrix4x4& b)
{
  Matrix4x4 ret;

  for(size_t i = 0; i < 4; ++i) {
    Vector4D row = a.getRow(i);
		
    for(size_t j = 0; j < 4; ++j) {
      ret[i][j] = row[0] * b[0][j] + row[1] * b[1][j] + 
        row[2] * b[2][j] + row[3] * b[3][j];
    }
  }

  return ret;
}

inline std::ostream& operator <<(std::ostream& os, const Matrix4x4& M)
{
  return os << "[" << M[0][0] << " " << M[0][1] << " " 
            << M[0][2] << " " << M[0][3] << "]" << std::endl
            << "[" << M[1][0] << " " << M[1][1] << " " 
            << M[1][2] << " " << M[1][3] << "]" << std::endl
            << "[" << M[2][0] << " " << M[2][1] << " " 
            << M[2][2] << " " << M[2][3] << "]" << std::endl
            << "[" << M[3][0] << " " << M[3][1] << " " 
            << M[3][2] << " " << M[3][3] << "]";
}

#endif