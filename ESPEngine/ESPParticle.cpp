/**
 * ESPParticle.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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
	this->SetVelocity(vel);
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
	this->position = this->position + (dT * this->speed * this->velocityDir);
	this->currLifeElapsed += dT;
}

/**
 * Draw this particle as it is currently.
 */
void ESPParticle::Draw(const Matrix4x4& modelMat, const Matrix4x4& modelMatInv, 
                       const Matrix4x4& modelInvTMat, const Camera& camera, 
                       const ESP::ESPAlignment& alignment) {

	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle
	glPushMatrix();

	// Do any personal alignment transforms...
    Matrix4x4 alignmentMat;
    this->GetPersonalAlignmentTransform(modelMat, modelMatInv, modelInvTMat, camera, alignment, this->position, alignmentMat);
	glMultMatrixf(alignmentMat.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();
}

void ESPParticle::GetPersonalAlignmentTransform(const Matrix4x4& modelMat, const Matrix4x4& modelMatInv,
                                                const Matrix4x4& modelInvTMat, 
                                                const Camera& cam, const ESP::ESPAlignment alignment, 
                                                const Point3D& localPos, Matrix4x4& result) {

    static const Vector3D ZERO_VEC3D(0,0,0);

    Vector3D alignRightVec(1, 0, 0);
    Vector3D alignUpVec(0, 1, 0);
    Vector3D alignNormalVec(0, 0, 1);

    Point3D worldPos = modelMat * localPos;
    Point3D camPos   = cam.GetCurrentCameraPosition();

    // The normal vector is from the particle center to the eye
    alignNormalVec = camPos - worldPos;

    // Make sure there is a normal...
    if (alignNormalVec == ZERO_VEC3D) {
        // Default to having the particle point in the direction of the camera...
        alignNormalVec = cam.GetInvViewTransform() * -Camera::DEFAULT_FORWARD_VEC;
    }

    // Create the alignment transform matrix based off the given alignment...
    switch(alignment) {

        case ESP::AxisAligned: {
            if (!this->velocityDir.IsZero()) {
                alignUpVec = modelMat * this->velocityDir;
            }
            else {
                alignUpVec = modelMat * Vector3D(0, 1, 0);
            }

            alignRightVec = Vector3D::cross(alignUpVec, alignNormalVec);
            if (alignRightVec == ZERO_VEC3D) {
                alignRightVec = cam.GetInvViewTransform() * -Camera::DEFAULT_LEFT_VEC;
                if (alignRightVec == ZERO_VEC3D) {
                    alignRightVec = cam.GetInvViewTransform() * Camera::DEFAULT_UP_VEC;
                }
            }

            alignNormalVec	= Vector3D::cross(alignRightVec, alignUpVec);
            break;
        }

        case ESP::GlobalAxisAlignedX: {
            alignRightVec   = modelMatInv * Vector3D(1, 0, 0);
            alignUpVec      = Vector3D::cross(alignNormalVec, alignRightVec);
            alignNormalVec	= Vector3D::cross(alignRightVec, alignUpVec);
            break;
        }

        case ESP::ScreenAligned:
            alignUpVec     = cam.GetInvViewTransform() * Camera::DEFAULT_UP_VEC;
            alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            alignUpVec     = Vector3D::cross(alignNormalVec, alignRightVec);
            break;

        case ESP::ScreenAlignedFollowVelocity: {
            if (!this->velocityDir.IsZero()) {
                alignUpVec = modelMat * this->velocityDir;
            }
            else {
                alignUpVec = modelMat * Vector3D(0, 1, 0);
            }

            alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            if (alignRightVec == ZERO_VEC3D) {
                alignRightVec = cam.GetInvViewTransform() * -Camera::DEFAULT_LEFT_VEC;
                if (alignRightVec == ZERO_VEC3D) {
                    alignRightVec = cam.GetInvViewTransform() * Camera::DEFAULT_UP_VEC;
                }
            }

            alignNormalVec	= Vector3D::cross(alignRightVec, alignUpVec);
            break;
        }

        case ESP::ScreenAlignedGlobalUpVec: {
            alignNormalVec.CondenseAndNormalizeToLargestComponent();
            alignRightVec = modelMatInv * Vector3D(1, 0, 0);
            alignUpVec    = Vector3D::cross(alignNormalVec, alignRightVec);
            if (alignUpVec == ZERO_VEC3D) {
                alignUpVec      = modelMatInv * Vector3D(0, 1, 0);
                alignRightVec   = Vector3D::cross(alignUpVec, alignNormalVec);
            }
            alignNormalVec	= Vector3D::cross(alignRightVec, alignUpVec);
            break;
        }

        case ESP::ScreenPlaneAligned: {
            alignNormalVec.CondenseAndNormalizeToLargestComponent();
            alignUpVec     = modelMat * cam.GetInvViewTransform() * Camera::DEFAULT_UP_VEC;
            alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            if (alignRightVec == ZERO_VEC3D) {
                alignUpVec = modelMat * cam.GetInvViewTransform() * -Camera::DEFAULT_LEFT_VEC;
                alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            }
            alignUpVec = Vector3D::cross(alignNormalVec, alignRightVec);

            break;
        }

        case ESP::NoAlignment:
        default:
            result.SetRow(0, alignRightVec[0], alignUpVec[0], alignNormalVec[0], localPos[0]);
            result.SetRow(1, alignRightVec[1], alignUpVec[1], alignNormalVec[1], localPos[1]);
            result.SetRow(2, alignRightVec[2], alignUpVec[2], alignNormalVec[2], localPos[2]);
            result.SetRow(3, 0, 0, 0, 1);
            return;
    }

    // Multiply the particle's alignment basis by the world/model inverse transpose matrix
    alignNormalVec = modelInvTMat * alignNormalVec;
    alignNormalVec.Normalize();
    alignUpVec = modelInvTMat * alignUpVec;
    alignUpVec.Normalize();
    alignRightVec = modelInvTMat * alignRightVec;
    alignRightVec.Normalize();

    result.SetRow(0, alignRightVec[0], alignUpVec[0], alignNormalVec[0], localPos[0]);
    result.SetRow(1, alignRightVec[1], alignUpVec[1], alignNormalVec[1], localPos[1]);
    result.SetRow(2, alignRightVec[2], alignUpVec[2], alignNormalVec[2], localPos[2]);
    result.SetRow(3, 0, 0, 0, 1);
}