/**
 * Camera.cpp
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

#include "Camera.h"

const float Camera::FOV_ANGLE_IN_DEGS	= 50.0f;
const float Camera::NEAR_PLANE_DIST		= 0.01f;
const float Camera::FAR_PLANE_DIST		= 300.0f;

const Vector3D Camera::DEFAULT_FORWARD_VEC	= Vector3D(0, 0, -1);
const Vector3D Camera::DEFAULT_LEFT_VEC     = Vector3D(-1, 0, 0);
const Vector3D Camera::DEFAULT_UP_VEC       = Vector3D(0, 1, 0);

float Camera::aspectRatio = 0.0f;
int Camera::windowWidth   = 0;
int Camera::windowHeight  = 0;

Camera::Camera() : shakeVar(0.0), shakeTimeElapsed(0.0), shakeTimeTotal(0.0), 
shakeMagnitude(0,0,0), shakeSpeed(0), fovAngleInDegrees(FOV_ANGLE_IN_DEGS) {
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