#include "ESPParticle.h"

#include "../BlammoEngine/Camera.h"

const Vector3D ESPParticle::PARTICLE_UP_VEC				= Vector3D(0, 1, 0);
const Vector3D ESPParticle::PARTICLE_NORMAL_VEC		= Vector3D(0, 0, 1);
const Vector3D ESPParticle::PARTICLE_RIGHT_VEC		= Vector3D::cross(PARTICLE_UP_VEC, PARTICLE_NORMAL_VEC);

unsigned int ESPParticle::numberOfParticleInsts = 0;
GLuint ESPParticle::particleDispList = 0;

// NOTE: All particles are created as if they were already dead
ESPParticle::ESPParticle() : 
totalLifespan(0.0), currLifeElapsed(0.0), size(1.0f, 1.0f), initSize(1.0f, 1.0f), colour(1,1,1), alpha(1.0f), rotation(0.0f) {
	if (ESPParticle::numberOfParticleInsts == 0) {
		// First instance of a particle - make a display list for particles
		ESPParticle::particleDispList = ESPParticle::GenerateParticleDispList();
		assert(ESPParticle::particleDispList != 0);
	}
	ESPParticle::numberOfParticleInsts++;
}

ESPParticle::~ESPParticle() {
	if (ESPParticle::numberOfParticleInsts == 1) {
		// Last instance of a particle - destroy the display list
		glDeleteLists(ESPParticle::particleDispList, 1);
		ESPParticle::particleDispList = 0;
	}
	ESPParticle::numberOfParticleInsts--;
}

/**
 * Static function for creating the display list for a particle.
 * Returns: The display list ID, 0 on fail.
 */
GLuint ESPParticle::GenerateParticleDispList() {
	GLuint dispListVal = glGenLists(1);
	glNewList(dispListVal, GL_COMPILE);
	glBegin(GL_QUADS);
		glNormal3i(PARTICLE_NORMAL_VEC[0], PARTICLE_NORMAL_VEC[1], PARTICLE_NORMAL_VEC[2]);
		glTexCoord2i(0, 0); glVertex2f(-0.5f, -0.5f);
		glTexCoord2i(1, 0); glVertex2f( 0.5f, -0.5f);
		glTexCoord2i(1, 1); glVertex2f( 0.5f,  0.5f);
		glTexCoord2i(0, 1); glVertex2f(-0.5f,  0.5f);
	glEnd();
	glEndList();

	return dispListVal;
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
void ESPParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
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
void ESPParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
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
	
	glCallList(ESPParticle::particleDispList);

	glPopMatrix();
}

Matrix4x4 ESPParticle::GetPersonalAlignmentTransform(const Camera& cam, const ESP::ESPAlignment alignment) {
	Vector3D alignRightVec	= Vector3D(1, 0, 0);
	Vector3D alignUpVec			= Vector3D(0, 1, 0);
	Vector3D alignNormalVec	= Vector3D(0, 0, 1);
	
	if (alignment == ESP::ViewPointAligned || alignment == ESP::AxisAligned) {
		float tempMVXfVals[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
		
		// Obtain JUST the world transform...
		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(tempMVXfVals);
		glMultMatrixf(cam.GetInvViewTransform().begin());
		glGetFloatv(GL_MODELVIEW_MATRIX, tempMVXfVals);
		glPopMatrix();

		//Matrix4x4 worldMatrix(tempMVXfVals);
		//Point3D worldSpacePosition = worldMatrix * this->position;
		
		// The camera starts facing the +z direction, the sprite starts facing the -z direction...
		Point3D camPos = cam.GetCurrentCameraPosition();
		Point3D currPos = -1.0f * Point3D(camPos[0], camPos[1], -camPos[2]);
		
		// The normal vector is from the particle center to the eye
		alignNormalVec = Vector3D(currPos[0], currPos[1], currPos[2]);
		// Make sure there is a normal...
		if (alignNormalVec == Vector3D(0,0,0)) {
			alignNormalVec = PARTICLE_NORMAL_VEC;
		}
		else {
			alignNormalVec = Vector3D::Normalize(alignNormalVec);
		}
	}

	// Create the alignment transform matrix based off the given alignment...
	switch(alignment) {
		case ESP::ViewPointAligned:
			// The particle is aligned to the view-point
			// - The up vector is the particle up vector
			alignUpVec			= PARTICLE_UP_VEC;
		
			// Find out if up and normal are parallel and fix in that case...
			alignRightVec		= Vector3D::cross(alignUpVec, alignNormalVec);
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

		default:
			break;
	}

	return Matrix4x4(alignRightVec, alignUpVec, alignNormalVec);
}