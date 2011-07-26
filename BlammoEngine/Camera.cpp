/**
 * Camera.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Camera.h"

const float Camera::FOV_ANGLE_IN_DEGS	= 50.0f;
const float Camera::NEAR_PLANE_DIST		= 0.01f;
const float Camera::FAR_PLANE_DIST		= 300.0f;

const Vector3D Camera::DEFAULT_FORWARD_VEC	= Vector3D(0, 0, -1);
const Vector3D Camera::DEFAULT_LEFT_VEC			= Vector3D(-1, 0, 0);
const Vector3D Camera::DEFAULT_UP_VEC				= Vector3D(0, 1, 0);

float Camera::aspectRatio = 0.0f;

Camera::Camera(int width, int height) : 	shakeVar(0.0), shakeTimeElapsed(0.0), shakeTimeTotal(0.0), 
shakeMagnitude(0,0,0), shakeSpeed(0), fovAngleInDegrees(FOV_ANGLE_IN_DEGS),
windowWidth(width), windowHeight(height) {
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

/**
 * Sets the camera transform based on the given m, m will become the
 * transformation to get from camera to world coordinates and the inverse
 * will become the view inverse matrix.
 */
void Camera::SetTransform(const Matrix4x4& m) {
	this->viewMatrix = m;
	this->invViewMatrix = m.inverse();
}

/**
 * Moves the camera along the given vector (in camera coords) without changing the view
 * direction or up direction.
 */
void Camera::Move(const Vector3D &v) {
	Matrix4x4 translation			= Matrix4x4::translationMatrix(v);
	Matrix4x4 invTranslation	= Matrix4x4::translationMatrix(-1.0f * v);

	this->viewMatrix		= invTranslation * this->viewMatrix;
	this->invViewMatrix	= this->invViewMatrix * translation;
}

/**
 * Rotates the camera along the given axis ('x', 'y' or 'z'), in camera coords.
 */
void Camera::Rotate(char axis, float degs) {
	Matrix4x4 rotation		= Matrix4x4::rotationMatrix(axis, degs, true);
	Matrix4x4 invRotation	= Matrix4x4::rotationMatrix(axis, -degs, true);

	this->viewMatrix		= invRotation * this->viewMatrix;
	this->invViewMatrix	= this->invViewMatrix * rotation;
}