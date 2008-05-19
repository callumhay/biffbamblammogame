#include "Matrix.h"

static void swaprows(Matrix4x4& a, size_t r1, size_t r2)
{
  std::swap(a[r1][0], a[r2][0]);
  std::swap(a[r1][1], a[r2][1]);
  std::swap(a[r1][2], a[r2][2]);
  std::swap(a[r1][3], a[r2][3]);
}

static void dividerow(Matrix4x4& a, size_t r, float fac)
{
  a[r][0] /= fac;
  a[r][1] /= fac;
  a[r][2] /= fac;
  a[r][3] /= fac;
}

static void submultrow(Matrix4x4& a, size_t dest, size_t src, float fac)
{
  a[dest][0] -= fac * a[src][0];
  a[dest][1] -= fac * a[src][1];
  a[dest][2] -= fac * a[src][2];
  a[dest][3] -= fac * a[src][3];
}

Matrix4x4 Matrix4x4::invert() const
{
  /* The algorithm is plain old Gauss-Jordan elimination 
     with partial pivoting. */

  Matrix4x4 a(*this);
  Matrix4x4 ret;

  /* Loop over cols of a from left to right, 
     eliminating above and below diag */

  /* Find largest pivot in column j among rows j..3 */
  for(size_t j = 0; j < 4; ++j) { 
    size_t i1 = j; /* Row with largest pivot candidate */
    for(size_t i = j + 1; i < 4; ++i) {
      if(fabs(a[i][j]) > fabs(a[i1][j])) {
        i1 = i;
      }
    }

    /* Swap rows i1 and j in a and ret to put pivot on diagonal */
    swaprows(a, i1, j);
    swaprows(ret, i1, j);

    /* Scale row j to have a unit diagonal */
    if(a[j][j] == 0.0) {
      // Theoretically throw an exception.
      return ret;
    }

    dividerow(ret, j, a[j][j]);
    dividerow(a, j, a[j][j]);

    /* Eliminate off-diagonal elems in col j of a, doing identical 
       ops to b */
    for(size_t i = 0; i < 4; ++i) {
      if(i != j) {
        submultrow(ret, i, j, a[i][j]);
        submultrow(a, i, j, a[i][j]);
      }
    }
  }

  return ret;
}

Matrix4x4 Matrix4x4::translationMatrix(const Vector3D &translation){
  // Pretty easy, just put the displacement as the last column in the matrix
  Matrix4x4 t(Vector4D(1, 0, 0, translation[0]),
	      Vector4D(0, 1, 0, translation[1]),
	      Vector4D(0, 0, 1, translation[2]),
	      Vector4D(0, 0, 0, 1));

  return t;
}

Matrix4x4 Matrix4x4::rotationMatrix(char axis, float angle, bool inDegrees){
	
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
    break;
  }

  // Create and return the matrix based on the values calculated
  Matrix4x4 r(row1, row2, row3, row4);
  return r;
}

Matrix4x4 Matrix4x4::scaleMatrix(const Vector3D &scale){
  // Pretty easy, scales just make up the diagonal of the matrix
  Matrix4x4 s(Vector4D(scale[0], 0, 0, 0),
	      Vector4D(0, scale[1], 0, 0),
	      Vector4D(0, 0, scale[2], 0),
	      Vector4D(0, 0, 0, 1));

  return s;
}
