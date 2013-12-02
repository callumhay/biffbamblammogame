#include "ESPParticle.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

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
	Matrix4x4 personalAlignXF = this->GetPersonalAlignmentTransform(camera, alignment);
	
	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(personalAlignXF.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	
	GeometryMaker::GetInstance()->DrawQuad();

	glPopMatrix();
}

void ESPParticle::DrawAsPointSprite(const Camera& camera, const Vector3D& translation) {
	if (this->IsDead()) {
		return;
	}

	// Calculate distance from sprite to camera and use it to adjust sprite size...
	Point3D camPos = camera.GetCurrentCameraPosition();
	Vector3D vecToCam = camPos - (this->position + translation);
	
	float dist = vecToCam.length();
	float pointSize = (this->size[0] / 2.0) * static_cast<float>(camera.GetWindowHeight()) / (tan(Trig::degreesToRadians(camera.GetFOVAngleInDegrees() / 2.0)) * dist);
	pointSize = std::max<float>(minMaxPtSize[0], std::min<float>(minMaxPtSize[1], pointSize));
	glPointSize(pointSize);

	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	glBegin(GL_POINTS);
	glVertex3fv(this->position.begin());
	glEnd();
}

Matrix4x4 ESPParticle::GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment) {
	Vector3D alignRightVec	= Vector3D(1, 0, 0);
	Vector3D alignUpVec		= Vector3D(0, 1, 0);
	Vector3D alignNormalVec	= Vector3D(0, 0, 1);
	
	Point3D camPos = cam.GetCurrentCameraPosition();
	
	// Create the alignment transform matrix based off the given alignment...
	switch(alignment) {
		case ESP::ViewPointAligned:

			// The normal vector is from the particle center to the eye
			alignNormalVec = Vector3D(-camPos[0], camPos[1], camPos[2]);
			// Make sure there is a normal...
			if (alignNormalVec == Vector3D(0,0,0)) {
				alignNormalVec = PARTICLE_NORMAL_VEC;
			}
			else {
				alignNormalVec = Vector3D::Normalize(alignNormalVec);
			}

			// The particle is aligned to the view-point
			// - The up vector is the particle up vector
			alignUpVec = PARTICLE_UP_VEC;
		
			// Find out if up and normal are parallel and fix in that case...
			alignRightVec		= Vector3D::cross(alignUpVec, alignNormalVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::cross(cam.GetNormalizedUpVector(), alignNormalVec);
				if (alignRightVec == Vector3D(0,0,0)) {
					alignRightVec = PARTICLE_RIGHT_VEC;
				}
			}
			alignRightVec = Vector3D::Normalize(alignRightVec);
			alignUpVec    = Vector3D::Normalize(Vector3D::cross(alignNormalVec, alignRightVec));
			break;

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

		case ESP::ScreenAlignedFollowVelocity:
			alignNormalVec = -cam.GetNormalizedViewVector();
			alignUpVec     = Vector3D::Normalize(Vector3D(-this->velocity[0], this->velocity[1], -this->velocity[2]));
			alignRightVec  = Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
			break;

        case ESP::ScreenAlignedGlobalUpVec:

            alignNormalVec = -cam.GetNormalizedViewVector();
            alignUpVec     = Vector3D(0, 1, 0);
            alignRightVec  = Vector3D::cross(alignUpVec, alignNormalVec);
            if (alignRightVec.IsZero()) {
                alignUpVec    = Vector3D(0, 0, 1);
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