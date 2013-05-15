/**
 * GothicRomanticBossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */


#ifndef __GOTHICROMANTICBOSSMESH_H__
#define __GOTHICROMANTICBOSSMESH_H__

#include "BossMesh.h"

class GothicRomanticBoss;
class Mesh;
class ESPPointEmitter;

class GothicRomanticBossMesh : public BossMesh {
public:
    GothicRomanticBossMesh(GothicRomanticBoss* boss);
    ~GothicRomanticBossMesh();

    double ActivateIntroAnimation();

private:
    static const double INTRO_TIME_IN_SECS;

    GothicRomanticBoss* boss; // N.B., The pointed-to object doesn't belong to this
    
    Mesh* bodyMesh;
    Mesh* topPointMesh;
    Mesh* bottomPointMesh;
    Mesh* legMesh;

    Texture2D* circleGlowTex;
    AnimationMultiLerp<float> glowCirclePulseAnim;

    ESPPointEmitter* topPointSmokeEmitter;
    ESPPointEmitter* topPointFireEmitter;
    ESPPointEmitter* topPointExplodingEmitter;

    ESPPointEmitter* explodingLegEmitter;

    ESPPointEmitter* bottomPointSmokeEmitter;
    ESPPointEmitter* bottomPointFireEmitter;
    ESPPointEmitter* bottomPointExplodingEmitter;

    ESPPointEmitter* bodySmokeEmitter;
    ESPPointEmitter* bodyFireEmitter;
    ESPPointEmitter* bodyExplodingEmitter;

    // Intro animations and effects
    double introTimeCountdown;
    AnimationLerp<float> bottomPtGlowAlphaAnim;
    std::vector<AnimationLerp<float> > legGlowAlphaAnims;

    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawPostBodyEffects(double dT, const Camera& camera);

    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(GothicRomanticBossMesh);
};

#endif // __GOTHICROMANTICBOSSMESH_H__