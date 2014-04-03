/**
 * ESPParticle.cpp
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

#include "ESPParticle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

#include <Eigen/Geometry>

const Vector3D ESPParticle::PARTICLE_UP_VEC			= Vector3D(0, 1, 0);
const Vector3D ESPParticle::PARTICLE_NORMAL_VEC		= Vector3D(0, 0, 1);
const Vector3D ESPParticle::PARTICLE_RIGHT_VEC		= Vector3D::cross(PARTICLE_UP_VEC, PARTICLE_NORMAL_VEC);

float ESPParticle::minMaxPtSize[2];

// NOTE: All particles are created as if they were already dead
ESPParticle::ESPParticle() : 
totalLifespan(0.0), currLifeElapsed(0.0), size(1.0f, 1.0f), initSize(1.0f, 1.0f), colour(1,1,1), alpha(1.0f), rotation(0.0f) {
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, minMaxPtSize);
}

ESPParticle::~ESPParticle() {
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
void ESPParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size,
                         float rot, float totalLifespan) {

	// Set the members to reflect a 'new life'
	this->totalLifespan = totalLifespan;
	this->currLifeElapsed = 0.0f;
	this->size = size;
	this->initSize = size;
	this->rotation = rot;
	this->position = pos;
	this->velocity = vel;
}

/**
 * Called each frame with the delta time for that frame, this will
 * provide a slice of the lifetime of the particle.
 */
void ESPParticle::Tick(const double dT) {
	// Do nothing if the particle is dead
	if (this->IsDead()) {
		return;
	}

	// Update the particle accordingly...
	this->position = this->position + (dT * this->velocity);
	this->currLifeElapsed += dT;
}

/**
 * Draw this particle as it is currently.
 */
void ESPParticle::Draw(const Camera& camera, const ESP::ESPAlignment& alignment) {

	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle
	glPushMatrix();

	// Do any personal alignment transforms...
	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(this->GetPersonalAlignmentTransform(camera, alignment).begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();
}

Matrix4x4 ESPParticle::GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment) {
	Vector3D alignRightVec	= Vector3D(1, 0, 0);
	Vector3D alignUpVec		= Vector3D(0, 1, 0);
	Vector3D alignNormalVec	= Vector3D(0, 0, 1);
	
	Point3D camPos = cam.GetCurrentCameraPosition();
	
	// Create the alignment transform matrix based off the given alignment...
	switch(alignment) {
		
		case ESP::AxisAligned:
			// The normal vector is from the particle center to the eye
			alignNormalVec = Vector3D(camPos[0], -camPos[1], camPos[2]);
			// Make sure there is a normal...
			if (alignNormalVec == Vector3D(0,0,0)) {
				alignNormalVec = PARTICLE_NORMAL_VEC;
			}
			else {
				alignNormalVec = Vector3D::Normalize(alignNormalVec);
			}

			// The particle is aligned to its axis of velocity
			// - The normal vector is from the particle center to the eye	
			// - The up vector is the velocity vector of this particle
            if (this->velocity.length2() != 0) {
			    alignUpVec = Vector3D::Normalize(Vector3D(-this->velocity[0], this->velocity[1], -this->velocity[2]));
            }
            else {
                alignUpVec = Vector3D(0, 1, 0);
            }
			
            alignRightVec = Vector3D::cross(alignUpVec, alignNormalVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::cross(PARTICLE_RIGHT_VEC, alignNormalVec);
				if (alignRightVec == Vector3D(0,0,0)) {
					alignRightVec = Vector3D::cross(PARTICLE_NORMAL_VEC, alignNormalVec);
				}
			}
			alignRightVec = Vector3D::Normalize(alignRightVec);
			alignNormalVec	= Vector3D::Normalize(Vector3D::cross(alignRightVec, alignUpVec));
			break;

		case ESP::ScreenAligned:
			// The particle is aligned with the screen 
			// - The normal is the negation of the view plane normal (in world space)
			// - The up vector is the camera's up direction
			// - The right vector is just a cross product of the above two...

			alignNormalVec = -cam.GetNormalizedViewVector();
			alignUpVec     = cam.GetNormalizedUpVector();
			alignRightVec  = Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
			break;

        case ESP::ScreenAlignedFollowVelocity: {
            alignNormalVec = -cam.GetNormalizedViewVector();
            alignUpVec     = cam.GetNormalizedUpVector();
            alignRightVec  = Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
            
            Eigen::Matrix3f alignMatrix;
            alignMatrix << 
                alignRightVec[0], alignRightVec[1], alignRightVec[2],
                alignUpVec[0], alignUpVec[1], alignUpVec[2],
                alignNormalVec[0], alignNormalVec[1], alignNormalVec[2];

            Eigen::Quaternionf rot;
            rot.setFromTwoVectors(Eigen::Vector3f(0,1,0), Eigen::Vector3f(-this->velocity[0], this->velocity[1], -this->velocity[2]));
            rot.normalize();

            alignMatrix = alignMatrix*rot;

            return Matrix4x4(alignMatrix(0,0), alignMatrix(0,1), alignMatrix(0,2), 0,
                             alignMatrix(1,0), alignMatrix(1,1), alignMatrix(1,2), 0,
                             alignMatrix(2,0), alignMatrix(2,1), alignMatrix(2,2), 0,
                             0, 0, 0, 1);
        }
			//alignNormalVec = -cam.GetNormalizedViewVector();
			//alignUpVec     = Vector3D::Normalize(Vector3D(-this->velocity[0], this->velocity[1], this->velocity[2]));
			//alignRightVec  = Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
			//break;

        case ESP::ScreenAlignedGlobalUpVec:

            alignNormalVec = -cam.GetNormalizedViewVector();
            alignUpVec     = Vector3D(0, 1, 0);
            alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            if (alignRightVec.IsZero()) {
                alignUpVec    = Vector3D(0, 0, -1);
                alignRightVec = Vector3D::cross(alignUpVec, alignNormalVec);
                assert(!alignRightVec.IsZero());
            }

            alignRightVec.Normalize();

            break;

		case ESP::ViewPlaneAligned:
			// The particle is aligned to the view plane...
			// - The normal is the negation of the view plane normal (in world space)
			// - The up vector is the up vector of the particle (in world space)
			// - The right vector is just a cross product of the above two...

			// FIX THIS!!!!
			alignNormalVec = -cam.GetNormalizedViewVector();
			alignUpVec     = PARTICLE_UP_VEC;

			alignRightVec = Vector3D::cross(alignUpVec, alignNormalVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::cross(PARTICLE_RIGHT_VEC, alignNormalVec);
				if (alignRightVec == Vector3D(0,0,0)) {
					alignRightVec = Vector3D::cross(PARTICLE_NORMAL_VEC, alignNormalVec);
				}
			}
			alignRightVec = Vector3D::Normalize(alignRightVec);
			alignUpVec    = Vector3D::Normalize(Vector3D::cross(alignNormalVec, alignRightVec));
			break;

		case ESP::NoAlignment:
			break;

		default:
			break;
	}

	return Matrix4x4(alignRightVec, alignUpVec, alignNormalVec);
}