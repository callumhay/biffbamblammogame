/**
 * ESPShaderParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPShaderParticle.h"

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

ESPShaderParticle::ESPShaderParticle(CgFxEffectBase* effect) : ESPParticle(), shaderEffect(effect) {
	assert(effect != NULL);
}

ESPShaderParticle::~ESPShaderParticle() {
}

/**
 * Draw this particle as it is currently.
 */
void ESPShaderParticle::Draw(const Camera& camera, const ESP::ESPAlignment& alignment) {

	// Don't draw if dead...
	if (this->IsDead()) {
		return;
	}

	// Transform and draw the particle...
	glPushMatrix();

	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(this->GetPersonalAlignmentTransform(camera, alignment).begin());
	glRotatef(this->rotation, 0, 0, -1);
	glScalef(this->size[0], this->size[1], 1.0f);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);

	this->shaderEffect->Draw(camera, GeometryMaker::GetInstance()->GetQuadDL());

	glPopMatrix();
}