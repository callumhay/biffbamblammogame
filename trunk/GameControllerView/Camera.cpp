#include "Camera.h"

const float Camera::FOV_ANGLE_IN_DEGS	= 45.0f;
const float Camera::NEAR_PLANE_DIST		= 0.01f;
const float Camera::FAR_PLANE_DIST		= 250.0f;

const Vector3D Camera::DEFAULT_FORWARD_VEC	= Vector3D(0, 0, -1);
const Vector3D Camera::DEFAULT_LEFT_VEC			= Vector3D(-1, 0, 0);
const Vector3D Camera::DEFAULT_UP_VEC				= Vector3D(0, 1, 0);

Camera::Camera() {
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