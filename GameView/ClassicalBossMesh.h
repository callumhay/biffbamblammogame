/**
 * ClassicalBossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __CLASSICALBOSSMESH_H__
#define __CLASSICALBOSSMESH_H__

#include "BossMesh.h"

#include "../BlammoEngine/Light.h"

class ClassicalBoss;
class Mesh;
class ESPPointEmitter;

class ClassicalBossMesh : public BossMesh {
public:
    ClassicalBossMesh(ClassicalBoss* boss);
    ~ClassicalBossMesh();

    double ActivateIntroAnimation();

private:
    static const double INTRO_TIME_IN_SECS;
    static const float INTRO_SPARKLE_TIME_IN_SECS;

    ClassicalBoss* boss; // N.B., The pointed-to object doesn't belong to this

    // Mesh assets for the various body parts of the boss
    Mesh* eyeMesh;
    Mesh* pedimentMesh;
    Mesh* tablatureMesh;
    Mesh* columnMesh;
    Mesh* baseMesh;
    Mesh* armSquareMesh;
    Mesh* restOfArmMesh;

    // Intro animations and effects
    double introTimeCountdown;

    Texture2D* sparkleTex;
    ESPParticleScaleEffector particleGrowToSize;
    ESPParticleRotateEffector particleTwirl;
    ESPParticleColourEffector particleFader;
    ESPPointEmitter introSparkle;
    
    Texture2D* glowTex;
    
    //ESPPointEmitter introLenseFlare;

    // Visual effects for the boss when it's alive and active
    ESPParticleScaleEffector eyeGlowPulser;
    ESPPointEmitter eyePulseGlow;

    ESPPointEmitter* leftArmFireEmitter;
    ESPPointEmitter* leftArmSmokeEmitter;
    ESPPointEmitter* rightArmFireEmitter;
    ESPPointEmitter* rightArmSmokeEmitter;
    ESPPointEmitter* eyeSmokeEmitter;
    ESPPointEmitter* eyeFireEmitter;

    ESPPointEmitter* leftArmExplodingEmitter;
    ESPPointEmitter* rightArmExplodingEmitter;
    std::vector<ESPPointEmitter*> columnExplodingEmitters;
    std::vector<ESPPointEmitter*> tablatureExplodingEmitters;
    ESPPointEmitter* baseExplodingEmitter;
    ESPPointEmitter* pedimentExplodingEmitter;
    ESPPointEmitter* eyeExplodingEmitter;

    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawPostBodyEffects(double dT, const Camera& camera);

    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(ClassicalBossMesh);
};

#endif // __CLASSICALBOSSMESH_H__