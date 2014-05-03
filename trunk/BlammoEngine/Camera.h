/**
 * Camera.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
			
			this->shakeVar += dT * this->shakeSpeed;
			if (this->shakeVar > M_PI) {
				this->shakeVar -= M_PI;
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

            this->shakeVar += dT * this->shakeSpeed;
            if (this->shakeVar > M_PI) {
                this->shakeVar -= M_PI;
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

	/**
     * Sets the camera transform based on the given m, m will become the
     * transformation to get from camera to world coordinates and the inverse
     * will become the view inverse matrix.
     */
    void SetTransform(const Matrix4x4& m) {
	    this->viewMatrix = m;
	    this->invViewMatrix = m.inverse();
    }
    void SetInvTransform(const Matrix4x4& m) {
        this->invViewMatrix = m;
        this->viewMatrix = m.inverse();
    }

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

	void ApplyCameraTransform() const {
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

    static void PushOrthoWindowCoords(float viewportWidth, float viewportHeight) {
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glViewport(0, 0, viewportWidth, viewportHeight);
        gluOrtho2D(0, viewportWidth, 0, viewportHeight);
        glPopAttrib();
    }

};
#endif