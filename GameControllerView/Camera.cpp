#include "Camera.h"

Camera::Camera() {
}

void Camera::Move(const Vector3D &v) {
	Matrix4x4 translation			= Matrix4x4::translationMatrix(v);
	Matrix4x4 invTranslation	= Matrix4x4::translationMatrix(-1.0f * v);

	this->viewMatrix		= invTranslation * this->viewMatrix;
	this->invViewMatrix	= this->invViewMatrix * translation;
}

void Camera::Rotate(char axis, float degs) {
	
}