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

	// Frustum properties
	float fovInDegs, nearPlaneDist, farPlaneDist;
	int width, height;

public:
	Camera(float fov, float nearDist, float farDist, int w, int h);

	void Move(const Vector3D &v);
	void Rotate(char axis, float degs);
	
	/*
	 * Set the viewport and projection transforms.
	 * Precondition: h > 0.
	 */
	void SetViewportAndProjectionTransform(int w, int h) {
			assert(h > 0);
			this->width = w;
			this->height = h;
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(this->fovInDegs, ((double)w) / ((double)h), this->nearPlaneDist, this->farPlaneDist);
	}

	void SetCameraTransform() {
		glMultMatrixf(this->viewMatrix.begin());
	}


};
#endif