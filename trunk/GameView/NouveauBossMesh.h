/**
 * NouveauBossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __NOUVEAUBOSSMESH_H__
#define __NOUVEAUBOSSMESH_H__

#include "BossMesh.h"

class NouveauBoss;
class Mesh;

class NouveauBossMesh : public BossMesh {
public:
    NouveauBossMesh(NouveauBoss* boss);
    ~NouveauBossMesh();

    // Inherited from BossMesh
    double ActivateIntroAnimation();

private:
    static const double INTRO_TIME_IN_SECS;

    NouveauBoss* boss; // N.B., The pointed-to object doesn't belong to this

    Mesh* bodyMesh;
    Mesh* bottomCurlMesh;
    Mesh* bottomHexSphereMesh;
    Mesh* sideCurlsMesh;
    Mesh* sideSphereMesh;
    Mesh* sideSphereFrillsMesh;
    Mesh* sideSphereHolderCurlMesh;
    Mesh* topDomeMesh;
    Mesh* topGazeboMesh;
    Mesh* topSphereMesh;

    Texture2D* circleGlowTex;

    ESPPointEmitter* topGazeboSmokeEmitter;
    ESPPointEmitter* topGazeboFireEmitter;
    ESPPointEmitter* topGazeboExplodingEmitter;
    ESPPointEmitter* topDomeExplodingEmitter;

    ESPPointEmitter* bodySmokeEmitter;
    ESPPointEmitter* bodyFireEmitter;
    ESPPointEmitter* bodyExplodingEmitter;

    ESPPointEmitter* leftArmExplodingEmitter;
    ESPPointEmitter* rightArmExplodingEmitter;

    AnimationMultiLerp<float> glowCirclePulseAnim;
    AnimationMultiLerp<Colour> frillColourAnim;

    // Intro animations and effects
    double introTimeCountdown;
    AnimationLerp<float> leftSideTopGlowAnim;
    AnimationLerp<float> leftSideBottomGlowAnim;
    AnimationLerp<float> rightSideTopGlowAnim;
    AnimationLerp<float> rightSideBottomGlowAnim;
    AnimationLerp<float> bottomGlowAnim;

    //void DrawPreBodyEffects(double dT, const Camera& camera);
    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);
    void DrawPostBodyEffects(double dT, const Camera& camera);

    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(NouveauBossMesh);
};
#endif // __NOUVEAUBOSSMESH_H__