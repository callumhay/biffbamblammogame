#include "Camera.h"

Camera::Camera(float fov, float nearDist, float farDist, int w, int h):
fovInDegs(fov), nearPlaneDist(nearDist), farPlaneDist(farDist), width(w), height(h) {
}

void Camera::Move(const Vector3D &v) {
	
}

void Camera::Rotate(char axis, float degs) {
	
}