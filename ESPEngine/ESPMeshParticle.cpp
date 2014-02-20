/**
 * ESPMeshParticle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPMeshParticle.h"
#include "../BlammoEngine/Mesh.h"

ESPMeshParticle::ESPMeshParticle(Mesh* mesh) : 
ESPParticle(), mesh(mesh) {
    assert(mesh != NULL);
}

ESPMeshParticle::~ESPMeshParticle() {
    this->mesh = NULL;
}

void ESPMeshParticle::Draw(const Camera& camera, const ESP::ESPAlignment& alignment) {
    // Don't draw if dead...
    if (this->IsDead()) {
        return;
    }

    glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->alpha);

    // Transform and draw the particle
    glPushMatrix();

    // Personal transforms...
    glTranslatef(this->position[0], this->position[1], this->position[2]);
    glRotatef(this->rotation, 0, 0, -1);
    glScalef(this->size[0], this->size[1], 1.0f);
    this->mesh->Draw(camera);

    glPopMatrix();
}