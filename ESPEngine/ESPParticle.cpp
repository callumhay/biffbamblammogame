#include "ESPParticle.h"

int			ESPParticle::NUM_INSTANCES					= 0;
GLuint	ESPParticle::PARTICLE_QUAD_DISPLIST	= 0;

// NOTE: All particles are created as if they were already dead
ESPParticle::ESPParticle() : 
totalLifespan(0.0), currLifeElapsed(0.0), size(1.0f) {
	
	if (ESPParticle::NUM_INSTANCES == 0) {
		// Inline: Creation of the first ESPParticle instance
		
		// Create a display list of a unit quad, centered at the origin and
		// facing in the z direction.
		ESPParticle::PARTICLE_QUAD_DISPLIST = glGenLists(1); 
		glNewList(ESPParticle::PARTICLE_QUAD_DISPLIST, GL_COMPILE);
			glBegin(GL_TRIANGLES);
				glTexCoord2d(0, 0); glVertex2f(-0.5f, -0.5f);
				glTexCoord2d(1, 0); glVertex2f(0.5f, -0.5f);
				glTexCoord2d(1, 1); glVertex2f(0.5f, 0.5f);
				glTexCoord2d(0, 1); glVertex2f(-0.5f, 0.5f);
			glEnd();
		glEndList();
	}
	ESPParticle::NUM_INSTANCES++;
}

ESPParticle::~ESPParticle() {
	if (ESPParticle::NUM_INSTANCES == 1) {
		// Inline: Deleting the last ESPParticle instance
		glDeleteLists(ESPParticle::PARTICLE_QUAD_DISPLIST, 1);
	}
	ESPParticle::NUM_INSTANCES--;
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
void ESPParticle::Draw() {
	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	glPushMatrix();
	glScalef(this->size, this->size, this->size);
	glCallList(ESPParticle::PARTICLE_QUAD_DISPLIST);
	glPopMatrix();
}