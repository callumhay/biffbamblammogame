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

	Matrix4x4 GetViewTransform() const {
		return this->viewMatrix;
	}

	Matrix4x4 GetInvViewTransform() const {
		return this->invViewMatrix;
	}

	void Reset() {
		this->viewMatrix		= Matrix4x4();
		this->invViewMatrix = Matrix4x4();
	}

	void Move(const Vector3D &v);
	void Rotate(char axis, float degs);

	void ApplyCameraTransform() {
		glMultMatrixf(this->viewMatrix.begin());
	}

	// Functions for changing the current view transform to a full ortho2D,
	// such that all coordinates are in window coords
	static void PushWindowCoords() {
		glPushAttrib(GL_TRANSFORM_BIT);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
		glPopAttrib();
	}
	static void PopWindowCoords() {
		glPushAttrib(GL_TRANSFORM_BIT);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}

};
#endif