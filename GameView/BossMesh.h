/**
 * BossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSMESH_H__
#define __BOSSMESH_H__

#include "../GameModel/GameWorld.h"
#include "../ESPEngine/ESP.h"

class Camera;
class BasicPointLight;
class Boss;
class Texture2D;

/**
 * The abstract superclass for all meshes/visuals of bosses in the game.
 */
class BossMesh {
public:
    BossMesh();
    virtual ~BossMesh();

    static BossMesh* Build(const GameWorld::WorldStyle& style, Boss* boss);
    
    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

    virtual double ActivateIntroAnimation() = 0;
    double ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera);

protected:
    // Shared visual effects and textures for bosses
    std::vector<Texture2D*> smokeTextures;
    Texture2D* explosionAnimTex;

    ESPParticleScaleEffector  particleLargeGrowth;
    ESPParticleScaleEffector  particleMediumGrowth;
    ESPParticleColourEffector particleFireColourFader;
    ESPParticleColourEffector smokeColourFader;
	ESPParticleRotateEffector rotateEffectorCW;
    ESPParticleRotateEffector rotateEffectorCCW;

    // Final explosion effect members
    bool finalExplosionIsActive;
    AnimationLerp<float> lineAnim;  // Length of each of the two lines that go out from the boss to the sides
    AnimationLerp<float> flashAnim; // The fullscreen flash half-height

    virtual void DrawPreBodyEffects(double dT, const Camera& camera);
    virtual void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) = 0;
    virtual void DrawPostBodyEffects(double dT, const Camera& camera) = 0;

    virtual Point3D GetBossFinalExplodingEpicenter() const = 0;

    ESPPointEmitter* BuildFireEmitter(float width, float height);
    ESPPointEmitter* BuildSmokeEmitter(float width, float height);
    ESPPointEmitter* BuildExplodingEmitter(float width, float height);


private:
    DISALLOW_COPY_AND_ASSIGN(BossMesh);
};

inline void BossMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    this->DrawPreBodyEffects(dT, camera);
    this->DrawBody(dT, camera, keyLight, fillLight, ballLight);
    this->DrawPostBodyEffects(dT, camera);
}

#endif // __BOSSMESH_H__