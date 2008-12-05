#include "ESPParticle.h"

#include "../BlammoEngine/Camera.h"

const Vector3D ESPParticle::PARTICLE_UP_VEC				= Vector3D(0, 1, 0);
const Vector3D ESPParticle::PARTICLE_NORMAL_VEC		= Vector3D(0, 0, 1);
const Vector3D ESPParticle::PARTICLE_RIGHT_VEC		= Vector3D::cross(PARTICLE_UP_VEC, PARTICLE_NORMAL_VEC);

// NOTE: All particles are created as if they were already dead
ESPParticle::ESPParticle() : 
totalLifespan(0.0), currLifeElapsed(0.0), size(1.0f), colour(1,1,1), alpha(1.0f) {
}

ESPParticle::~ESPParticle() {
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
void ESPParticle::Revive(const Point3D& pos, const Vector3D& vel, const float size, const float totalLifespan) {
	// Set the members to reflect a 'new life'
	this->totalLifespan = totalLifespan;
	this->currLifeElapsed = 0.0f;
	this->size = size;
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
void ESPParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle
	glPushMatrix();

	// Move the particle into position
	//glTranslatef(this->position[0], this->position[1], this->position[2]);

	// Do any personal alignment transforms...
	Matrix4x4 personalAlignXF = this->GetPersonalAlignmentTransform(camera, alignment);
	personalAlignXF[12]  += this->position[0]; 
  personalAlignXF[13]  += this->position[1]; 
  personalAlignXF[14]  += this->position[2]; 
	glMultMatrixf(personalAlignXF.begin());

	float halfSize = 0.5f * this->size;
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);
	glBegin(GL_QUADS);
		glNormal3i(PARTICLE_NORMAL_VEC[0], PARTICLE_NORMAL_VEC[1], PARTICLE_NORMAL_VEC[2]);
		glTexCoord2i(0, 0); glVertex2f(-halfSize, -halfSize);
		glTexCoord2i(1, 0); glVertex2f( halfSize, -halfSize);
		glTexCoord2i(1, 1); glVertex2f( halfSize,  halfSize);
		glTexCoord2i(0, 1); glVertex2f(-halfSize,  halfSize);
	glEnd();

	glPopMatrix();
}

/**
 * Obtains the matrix transform for transforming particles
 * to the given alignment.
 * Returns: A transform matrix that will transform any particle such that it
 * adheres to the given alignment.
 */
Matrix4x4 ESPParticle::GetAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment) {
	Vector3D alignRightVec	= Vector3D(1, 0, 0);
	Vector3D alignUpVec			= Vector3D(0, 1, 0);
	Vector3D alignNormalVec	= Vector3D(0, 0, 1);

	// Create the alignment transform matrix based off the given alignment...
	switch(alignment) {
		case ESP::ScreenAligned:
			// The particle is aligned with the screen 
			// - The normal is the negation of the view plane normal (in world space)
			// - The up vector is the camera's up direction
			// - The right vector is just a cross product of the above two...

			alignNormalVec = -cam.GetNormalizedViewVector();
			alignUpVec			= cam.GetNormalizedUpVector();
			alignRightVec	= Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
			break;

		case ESP::ViewPlaneAligned:
			// The particle is aligned to the view plane...
			// - The normal is the negation of the view plane normal (in world space)
			// - The up vector is the up vector of the particle (in world space)
			// - The right vector is just a cross product of the above two...

			// FIX THIS!!!!
			alignNormalVec = -cam.GetNormalizedViewVector();
			alignUpVec			= PARTICLE_UP_VEC;

			alignRightVec	= Vector3D::cross(alignUpVec, alignNormalVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::MollerHughesPerpendicular(alignNormalVec);
			}
			alignRightVec		= Vector3D::Normalize(alignRightVec);
			alignUpVec			= Vector3D::Normalize(Vector3D::cross(alignNormalVec, alignRightVec));
			break;

		// The rest of these are per-particle transforms...
		case ESP::ViewPointAligned:
		case ESP::AxisAligned:
		default:
			break;
	}

	return Matrix4x4(alignRightVec, alignUpVec, alignNormalVec);
}

// TODO: MAKE THIS WORK...
Matrix4x4 ESPParticle::GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment) {
	Vector3D alignRightVec	= Vector3D(1, 0, 0);
	Vector3D alignUpVec			= Vector3D(0, 1, 0);
	Vector3D alignNormalVec	= Vector3D(0, 0, 1);
	
	float tempMVXfVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	
	// Obtain JUST the world transform...
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(cam.GetInvViewTransform().begin());
	glMultMatrixf(tempMVXfVals);
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
	glPopMatrix();

	Matrix4x4 worldMatrix(tempMVXfVals);
	Point3D currPos = worldMatrix *  this->position;
	currPos = cam.GetCurrentCameraPosition() - Vector3D(currPos[0], currPos[1], currPos[2]);
	
	// The normal vector is from the particle center to the eye
	alignNormalVec = Vector3D(currPos[0], currPos[1], currPos[2]);
	// Make sure there is a normal...
	if (alignNormalVec == Vector3D(0,0,0)) {
		alignNormalVec = PARTICLE_NORMAL_VEC;
	}
	else {
		alignNormalVec = Vector3D::Normalize(alignNormalVec);
	}

	// Create the alignment transform matrix based off the given alignment...
	switch(alignment) {
		case ESP::ViewPointAligned:
			// The particle is aligned to the view-point
			// - The up vector is the particle up vector
			alignUpVec			= PARTICLE_UP_VEC;
		
			// Find out if up and normal are parallel and fix in that case...
			alignRightVec		= Vector3D::cross(alignNormalVec, alignUpVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::MollerHughesPerpendicular(alignNormalVec);
			}
			alignRightVec		= Vector3D::Normalize(alignRightVec);
			alignUpVec			= Vector3D::Normalize(Vector3D::cross(alignNormalVec, alignRightVec));
			break;

		case ESP::AxisAligned:
			// The particle is aligned to its axis of velocity
			// - The normal vector is from the particle center to the eye	
			// - The up vector is the velocity vector of this particle
			alignUpVec			= Vector3D::Normalize(this->velocity);
			alignRightVec		= Vector3D::cross(alignNormalVec, alignUpVec);
			if (alignRightVec == Vector3D(0,0,0)) {
				alignRightVec = Vector3D::MollerHughesPerpendicular(alignNormalVec);
			}
			alignRightVec		= Vector3D::Normalize(alignRightVec);
			alignNormalVec	= Vector3D::Normalize(Vector3D::cross(alignRightVec, alignUpVec));
			break;

		// These are batch transforms...
		case ESP::ScreenAligned:
		case ESP::ViewPlaneAligned:
		default:
			break;
	}

	return Matrix4x4(alignRightVec, alignUpVec, alignNormalVec);
}