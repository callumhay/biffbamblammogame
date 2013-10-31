/**
 * DecoBossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __DECOBOSSMESH_H__
#define __DECOBOSSMESH_H__

#include "BossMesh.h"

class DecoBoss;
class Mesh;

class DecoBossMesh : public BossMesh {
public:
    DecoBossMesh(DecoBoss* boss, GameSound* sound);
    ~DecoBossMesh();

    // Inherited from BossMesh
    double ActivateIntroAnimation();

private:
    static const double INTRO_TIME_IN_SECS;

    DecoBoss* boss;  // N.B., The pointed-to object doesn't belong to this

    Mesh* coreMesh;
    Mesh* lightningRelayMesh;
    Mesh* gearMesh;
    Mesh* scopingArm1Mesh;
    Mesh* scopingArm2Mesh;
    Mesh* scopingArm3Mesh;
    Mesh* scopingArm4Mesh;
    Mesh* handMesh;
    Mesh* leftBodyMesh;
    Mesh* rightBodyMesh;

    Mesh* itemMesh;

    ESPPointEmitter* leftBodyExplodingEmitter;
    ESPPointEmitter* rightBodyExplodingEmitter;
    ESPPointEmitter* leftArmExplodingEmitter;
    ESPPointEmitter* rightArmExplodingEmitter;
    ESPPointEmitter* bodyExplodingEmitter;

    Texture2D* glowTex;
    double introTimeCountdown;
    std::vector<AnimationLerp<float> > eyeGlowAlphaAnims;
    SoundID lightningRelaySoundID;
    SoundID eyeGlowSoundID;
    AnimationLerp<float> lightningRelayAlphaAnim;

    AnimationMultiLerp<float> eyeFlashingAlphaAnim;

    Texture2D* flareGlowTex;
    Texture2D* lensFlareTex;
    ESPPointEmitter* lightningRelayEmitter;
    ESPParticleScaleEffector flarePulse;

    // Inherited functions
    //void DrawPreBodyEffects(double dT, const Camera& camera);
    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);
    void DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets);
    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(DecoBossMesh);
};

#endif // __DECOBOSSMESH_H__