/**
 * ESPMeshParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPMESHPARTICLE_H__
#define __ESPMESHPARTICLE_H__

#include "ESPParticle.h"

class Mesh;

class ESPMeshParticle : public ESPParticle {
public:
    ESPMeshParticle(Mesh* mesh);
    ~ESPMeshParticle();

    void Draw(const Camera& camera, const ESP::ESPAlignment& alignment);

private:
    Mesh* mesh; // Reference only, NOT owned by this!

    DISALLOW_COPY_AND_ASSIGN(ESPMeshParticle);
};

#endif // __ESPMESHPARTICLE_H__