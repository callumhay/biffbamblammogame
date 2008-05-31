#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../Utils/Includes.h"
#include "../Utils/Matrix.h"
#include "../Utils/Vector.h"

class Camera {

private:
	// View and inverse view matrices
	Matrix4x4 viewMatrix;
	Matrix4x4 invViewMatrix;

public:
	Camera();

	void Move(const Vector3D &v);
	void Rotate(char axis, float degs);

	void SetCameraTransform() {
		glMultMatrixf(this->viewMatrix.begin());
	}


};
#endif