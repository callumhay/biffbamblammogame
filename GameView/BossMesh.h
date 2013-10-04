/**
 * BossMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSMESH_H__
#define __BOSSMESH_H__

#include "../GameModel/GameWorld.h"
#include "../ESPEngine/ESP.h"
#include "../GameSound/SoundCommon.h"

class GameSound;
class Camera;
class BasicPointLight;
class Boss;
class Texture2D;
class GameAssets;
class LaserBeamSightsEffectInfo;
class PowerChargeEffectInfo;
class ExpandingHaloEffectInfo;
class SparkBurstEffectInfo;
class ElectricitySpasmEffectInfo;
class ElectrifiedEffectInfo;

/**
 * The abstract superclass for all meshes/visuals of bosses in the game.
 */
class BossMesh {
public:
    BossMesh(GameSound* sound);
    virtual ~BossMesh();

    static BossMesh* Build(const GameWorld::WorldStyle& style, Boss* boss, GameSound* sound);
    
    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
        const BasicPointLight& ballLight, const GameAssets* assets);

    virtual double ActivateIntroAnimation() = 0;
    double ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera);

    void ClearActiveEffects();
    void AddLaserBeamSightsEffect(const LaserBeamSightsEffectInfo& info);
    void AddBossPowerChargeEffect(const PowerChargeEffectInfo& info);
    void AddBossExpandingHaloEffect(const ExpandingHaloEffectInfo& info);
    void AddBossSparkBurstEffect(const SparkBurstEffectInfo& info);
    void AddElectricitySpasmEffect(const ElectricitySpasmEffectInfo& info);
    void AddElectrifiedEffect(const ElectrifiedEffectInfo& info);

protected:
    GameSound* sound;

    // Shared visual effects and textures for bosses
    std::vector<Texture2D*> smokeTextures;
    Texture2D* explosionAnimTex;
    Texture2D* squareTargetTex;
    Texture2D* haloTex;
    Texture2D* sparkleTex;
    Texture2D* lightningAnimTex;

    ESPParticleScaleEffector  particleSmallGrowth;
    ESPParticleScaleEffector  particleLargeGrowth;
    ESPParticleScaleEffector  particleMediumGrowth;
    ESPParticleScaleEffector  particleSuperGrowth;
    ESPParticleScaleEffector  particleMediumShrink;
    ESPParticleScaleEffector  particleShrinkToNothing;
    ESPParticleColourEffector particleFader;
    ESPParticleColourEffector particleFireColourFader;
    ESPParticleColourEffector smokeColourFader;
	ESPParticleRotateEffector rotateEffectorCW;
    ESPParticleRotateEffector rotateEffectorCCW;

    ESPParticleColourEffector laserSightTargetColourChanger;
    ESPParticleScaleEffector  laserSightBigToSmallSize;
    ESPParticleRotateEffector laserSightTargetRotateEffector;

    // Final explosion effect members
    bool finalExplosionIsActive;
    AnimationLerp<float> lineAnim;  // Length of each of the two lines that go out from the boss to the sides
    AnimationLerp<float> flashAnim; // The full-screen flash half-height
    SoundID finalExplosionSoundID;

    // Boss-specific effects
    std::list<ESPEmitter*> fgEffectsEmitters;
    std::map<const BossBodyPart*, std::list<ESPEmitter*> > fgAttachedEffectsEmitters;
    std::list<ESPEmitter*> bgEffectsEmitters;

    virtual void DrawPreBodyEffects(double dT, const Camera& camera);
    virtual void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets) = 0;
    virtual void DrawPostBodyEffects(double dT, const Camera& camera);

    virtual Point3D GetBossFinalExplodingEpicenter() const = 0;

    ESPPointEmitter* BuildFireEmitter(float width, float height, float sizeScaler = 1.0f);
    ESPPointEmitter* BuildSmokeEmitter(float width, float height, float sizeScaler = 1.0f);
    ESPPointEmitter* BuildExplodingEmitter(float volumeAmt, const AbstractBossBodyPart* bossBodyPart, float width, float height, float sizeScaler = 1.0f);

private:
    class ExplodingEmitterHandler : public ESPEmitterEventHandler {
    public:
        ExplodingEmitterHandler(GameSound* sound, const AbstractBossBodyPart* bossBodyPart, float volumeAmt);
        ~ExplodingEmitterHandler() {};

        void ParticleSpawnedEvent(const ESPParticle* particle);
        void ParticleDiedEvent(const ESPParticle*) {};

    private:
        float volumeLevel;
        GameSound* sound;
        const AbstractBossBodyPart* bossBodyPart;
    };
    std::list<ExplodingEmitterHandler*> explodingEmitterHandlers;

    DISALLOW_COPY_AND_ASSIGN(BossMesh);
};

inline void BossMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight, 
                           const GameAssets* assets) {

    this->DrawPreBodyEffects(dT, camera);
    this->DrawBody(dT, camera, keyLight, fillLight, ballLight, assets);
    this->DrawPostBodyEffects(dT, camera);
}

#endif // __BOSSMESH_H__