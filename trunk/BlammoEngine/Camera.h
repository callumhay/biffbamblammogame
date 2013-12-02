/**
 * Camera.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

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

	// Current field of view angle
	float fovAngleInDegrees;

	// Camera shake variables
	double shakeVar;
	double shakeTimeElapsed;
	double shakeTimeTotal;
	Vector3D shakeMagnitude;
	float shakeSpeed;

	static int windowWidth;
	static int windowHeight;
    static float aspectRatio;
        
public:
	static const float FOV_ANGLE_IN_DEGS;
	static const float NEAR_PLANE_DIST;
	static const float FAR_PLANE_DIST;

	static const Vector3D DEFAULT_FORWARD_VEC;
	static const Vector3D DEFAULT_LEFT_VEC;
	static const Vector3D DEFAULT_UP_VEC;

	Camera();

	void ApplyCameraShakeTransform(double dT) {
		if (this->shakeTimeElapsed < this->shakeTimeTotal) {

			// Decrease the magnitude of the shake based on expired time (use linear interpolation)...
			Vector3D lerpShakeMagMultiplier = this->shakeMagnitude - (this->shakeTimeElapsed  * this->shakeMagnitude / this->shakeTimeTotal);
			float shakeSine = sin(this->shakeVar);

            glTranslatef(
                lerpShakeMagMultiplier[0]*shakeSine,
                lerpShakeMagMultiplier[1]*shakeSine, 
                lerpShakeMagMultiplier[2]*shakeSine);
			
			this->shakeVar += dT * this->shakeSpeed * Randomizer::GetInstance()->RandomNumZeroToOne();
			if (this->shakeVar > 1.0) {
				this->shakeVar -= 1.0;
			}

			this->shakeTimeElapsed += dT;
		}
	}

    Vector3D TickAndGetCameraShakeTransform(double dT) {
        Vector3D lerpShakeMagMultiplier(0,0,0);

        if (this->shakeTimeElapsed < this->shakeTimeTotal) {

            // Decrease the magnitude of the shake based on expired time (use linear interpolation)...
            lerpShakeMagMultiplier = this->shakeMagnitude - (this->shakeTimeElapsed  * this->shakeMagnitude / this->shakeTimeTotal);
            lerpShakeMagMultiplier *= sin(this->shakeVar);

            this->shakeVar += dT * this->shakeSpeed * Randomizer::GetInstance()->RandomNumZeroToOne();
            if (this->shakeVar > 1.0) {
                this->shakeVar -= 1.0;
            }

            this->shakeTimeElapsed += dT;
        }

        return lerpShakeMagMultiplier;
    }

	static void SetWindowDimensions(int w, int h) {
		assert(w > 0 && h > 0);
		windowWidth = w;
		windowHeight = h;
        aspectRatio = static_cast<float>(w) / static_cast<float>(h);
	}
	static int GetWindowWidth() {
		return windowWidth;
	}
	static int GetWindowHeight() {
		return windowHeight;
	}
    inline static float Camera::GetAspectRatio() {
        return Camera::aspectRatio;
    }

	float GetFOVAngleInDegrees() const {
		return this->fovAngleInDegrees;
	}

	Matrix4x4 GenerateScreenAlignMatrix() const {
		Vector3D alignNormalVec = -this->GetNormalizedViewVector();
		Vector3D alignUpVec		  = this->GetNormalizedUpVector();
		Vector3D alignRightVec	= Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
		return Matrix4x4(alignRightVec, alignUpVec, alignNormalVec);
	}

	Vector3D GetNormalizedUpVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_UP_VEC); 
	}
	Vector3D GetNormalizedViewVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_FORWARD_VEC);
	}
	Vector3D GetNormalizedLeftVector() const {
		return Vector3D::Normalize(this->viewMatrix * DEFAULT_LEFT_VEC);
	}

	const Matrix4x4& GetViewTransform() const {
		return this->viewMatrix;
	}

	const Matrix4x4& GetInvViewTransform() const {
		return this->invViewMatrix;
	}

	/**
	 * Set the view matrix and its inverse to be the identity matrix.
	 */
	void Reset() {
		this->viewMatrix    = Matrix4x4();
		this->invViewMatrix = Matrix4x4();
	}

	void SetTransform(const Matrix4x4& m);
	void Move(const Vector3D &v);
	void Rotate(char axis, float degs);

	/**
	 * Set the camera to shake for some specified period of time at a given
	 * magnitude (in units) and speed (units/sec).
	 */
	void ApplyCameraShake(double lengthInSeconds, const Vector3D& shakeDirMag, float speed) {
		// If we're still in the middle of a shake check to see which has the bigger magnitude and go with whichever one does...
		if (this->shakeTimeElapsed < this->shakeTimeTotal && this->shakeMagnitude.length2() > shakeDirMag.length2()) {
			return;
		}

		// Change to the given shake...
		this->shakeTimeElapsed = 0.0;
		this->shakeTimeTotal   = lengthInSeconds;
		
		this->shakeMagnitude = Vector3D(shakeDirMag[0], shakeDirMag[1], shakeDirMag[2]);
		this->shakeSpeed = speed;
	}
	// Clear the camera shake animation completely
	void ClearCameraShake() {
		this->shakeVar = 0.0;
		this->shakeTimeElapsed = 0.0;
		this->shakeTimeTotal = 0.0; 
		this->shakeMagnitude = Vector3D(0,0,0);
		this->shakeSpeed = 0;
	}

	void ApplyCameraTransform() {
		glMultMatrixf(this->viewMatrix.begin());
	}

	Point3D GetCurrentCameraPosition() const {
		return this->invViewMatrix.getTranslationPt3D();
	}

	// Functions for setting the camera perspective
	void SetPerspectiveWithFOV(float fovAngleInDegs) {
		this->fovAngleInDegrees = fovAngleInDegs;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, windowWidth, windowHeight);
		gluPerspective(this->fovAngleInDegrees, static_cast<double>(windowWidth) / 
            static_cast<double>(windowHeight), NEAR_PLANE_DIST, FAR_PLANE_DIST);
	}

	void SetPerspective() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, windowWidth, windowHeight);
		gluPerspective(this->fovAngleInDegrees, static_cast<double>(windowWidth) / 
            static_cast<double>(windowHeight), NEAR_PLANE_DIST, FAR_PLANE_DIST);
	}

	static void PushWindowCoords() {
		glPushAttrib(GL_TRANSFORM_BIT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
        gluOrtho2D(0,Camera::GetWindowWidth(),0,Camera::GetWindowHeight());
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