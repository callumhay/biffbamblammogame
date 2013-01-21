/**
 * ClassicalBossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CLASSICALBOSSMESH_H__
#define __CLASSICALBOSSMESH_H__

#include "BossMesh.h"

class ClassicalBoss;
class Mesh;
class ESPPointEmitter;

class ClassicalBossMesh : public BossMesh {
public:
    ClassicalBossMesh(ClassicalBoss* boss);
    ~ClassicalBossMesh();

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

private:
    ClassicalBoss* boss; // N.B., The pointed-to object doesn't belong to this

    // Mesh assets for the various body parts of the boss
    Mesh* eyeMesh;
    Mesh* pedimentMesh;
    Mesh* tablatureMesh;
    Mesh* columnMesh;
    Mesh* baseMesh;
    Mesh* armSquareMesh;
    Mesh* restOfArmMesh;

    // Visual effects for the boss
    ESPPointEmitter* leftArmSmokeEmitter;
    ESPPointEmitter* rightArmSmokeEmitter;

    ESPPointEmitter* leftArmExplodingEmitter;
    ESPPointEmitter* rightArmExplodingEmitter;

    void DrawEffects(double dT, const Camera& camera);

    DISALLOW_COPY_AND_ASSIGN(ClassicalBossMesh);
};

#endif // __CLASSICALBOSSMESH_H__