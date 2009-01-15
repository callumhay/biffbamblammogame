#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "BasicIncludes.h"
#include "Matrix.h"
#include "Vector.h"
#include "Point.h"

class Camera {

private:
	// View and inverse view matrices
	Matrix4x4 viewMatrix;
	Matrix4x4 invViewMatrix;

	// Camera shake variables
	double shakeVar;
	double shakeTimeElapsed;
	double shakeTimeTotal;
	Vector3D shakeMagnitude;
	unsigned int shakeSpeed;

	void ApplyCameraShakeTransform(double dT) {
		if (this->shakeTimeElapsed < this->shakeTimeTotal) {
		
			glTranslatef(this->shakeMagnitude[0]*sin(this->shakeVar), 
									 this->shakeMagnitude[1]*sin(this->shakeVar), 
									 this->shakeMagnitude[2]*sin(this->shakeVar));
			
			this->shakeVar += dT * shakeSpeed * Randomizer::GetInstance()->RandomNumZeroToOne();
			if (this->shakeVar > 1.0) {
				this->shakeVar -= 1.0;
			}

			this->shakeTimeElapsed += dT;
		}
	}

public:
	static const float FOV_ANGLE_IN_DEGS;
	static const float NEAR_PLANE_DIST;
	static const float FAR_PLANE_DIST;
	static const Vector3D DEFAULT_FORWARD_VEC;
	static const Vector3D DEFAULT_LEFT_VEC;
	static const Vector3D DEFAULT_UP_VEC;

	Camera();

	Vector3D GetNormalizedUpVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_UP_VEC); 
	}
	Vector3D GetNormalizedViewVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_FORWARD_VEC);
	}
	Vector3D GetNormalizedLeftVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_LEFT_VEC);
	}

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

	/**
	 * Set the camera to shake for some specified period of time at a given
	 * magnitude (in units) and speed (units/sec).
	 */
	void SetCameraShake(double lengthInSeconds, const Vector3D& shakeDirMag, unsigned int speed) {
		this->shakeTimeElapsed = 0.0;
		this->shakeTimeTotal	 = lengthInSeconds;
		this->shakeMagnitude = shakeDirMag;
		this->shakeSpeed = speed;
	}

	void ApplyCameraTransform(double dT) {
		glMultMatrixf(this->viewMatrix.begin());
		this->ApplyCameraShakeTransform(dT);
	}

	Point3D GetCurrentCameraPosition() const {
		return this->invViewMatrix.getTranslation();
	}

	// Function for setting the camera perspective
	static void SetPerspective(int w, int h) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(FOV_ANGLE_IN_DEGS, ((double)w) / ((double)h), NEAR_PLANE_DIST, FAR_PLANE_DIST);
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