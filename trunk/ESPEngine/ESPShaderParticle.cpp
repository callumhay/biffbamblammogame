#include "ESPShaderParticle.h"

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Camera.h"

ESPShaderParticle::ESPShaderParticle(CgFxEffectBase* effect) : ESPParticle(), shaderEffect(effect) {
	assert(effect != NULL);
}

ESPShaderParticle::~ESPShaderParticle() {
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
void ESPShaderParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
	// Set the members to reflect a 'new life'
	ESPParticle::Revive(pos, vel, size, rot, totalLifespan);
}

/**
 * Called each frame with the delta time for that frame, this will
 * provide a slice of the lifetime of the particle.
 */
void ESPShaderParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);
}

/**
 * Draw this particle as it is currently.
 */
void ESPShaderParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle...
	Matrix4x4 personalAlignXF = this->GetPersonalAlignmentTransform(camera, alignment);

	// Draw the font itself
	glPushMatrix();

	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(personalAlignXF.begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4d(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);

	this->shaderEffect->Draw(camera, ESPParticle::particleDispList);

	glPopMatrix();

}