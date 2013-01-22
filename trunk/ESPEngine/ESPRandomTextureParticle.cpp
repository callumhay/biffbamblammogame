/**
 * ESPRandomTextureParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPRandomTextureParticle.h"
#include "../BlammoEngine/Algebra.h"

ESPRandomTextureParticle::ESPRandomTextureParticle(const std::vector<Texture2D*>& textures) :
currSelectedTexIdx(0), textures(textures) {
    assert(!textures.empty());
    this->SelectRandomTexture();
}

ESPRandomTextureParticle::~ESPRandomTextureParticle() {
}

void ESPRandomTextureParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
    ESPParticle::Revive(pos, vel, size, rot, totalLifespan);
    this->SelectRandomTexture();
}

void ESPRandomTextureParticle::Draw(const Camera& camera, const ESP::ESPAlignment alignment) {
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
	
    const Texture2D* currTexture = this->textures[this->currSelectedTexIdx];

    currTexture->BindTexture();
	glBegin(GL_QUADS);
	glNormal3i(0, 0, 1);
	glTexCoord2i(0, 0); glVertex2f(-0.5f, -0.5f);
	glTexCoord2i(1, 0); glVertex2f( 0.5f, -0.5f);
	glTexCoord2i(1, 1);    glVertex2f( 0.5f,  0.5f);
	glTexCoord2i(0, 1);    glVertex2f(-0.5f,  0.5f);
	glEnd();
    currTexture->UnbindTexture();

	glPopMatrix();
}

void ESPRandomTextureParticle::SelectRandomTexture() {
    this->currSelectedTexIdx = Randomizer::GetInstance()->RandomUnsignedInt() % this->textures.size();
}