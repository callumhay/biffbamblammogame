/**
 * BossMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOSSMESH_H__
#define __BOSSMESH_H__

#include "../GameModel/GameWorld.h"
#include "../ESPEngine/ESP.h"
#include "../GameSound/SoundCommon.h"
#include "EffectUpdateStrategy.h"

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
class SummonPortalsEffectInfo;
class BossTeleportEffectInfo;
class CgFxBossWeakpoint;
class EffectUpdateStrategy;

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
    void DrawLaterPassEffects(double dT, const Camera& camera);

    virtual double ActivateIntroAnimation() = 0;
    double ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera);

    void ClearActiveEffects();
    void AddLaserBeamSightsEffect(const LaserBeamSightsEffectInfo& info);
    void AddBossPowerChargeEffect(const PowerChargeEffectInfo& info);
    void AddBossExpandingHaloEffect(const ExpandingHaloEffectInfo& info);
    void AddBossSparkBurstEffect(const SparkBurstEffectInfo& info);
    void AddElectricitySpasmEffect(const ElectricitySpasmEffectInfo& info);
    void AddElectrifiedEffect(const ElectrifiedEffectInfo& info);
    void AddSummonPortalEffect(const SummonPortalsEffectInfo& info);
    void AddTeleportEffect(const BossTeleportEffectInfo& info);

protected:
    GameSound* sound;

    // Weak point material used by all bosses
    CgFxBossWeakpoint* weakpointMaterial;

    // Shared visual effects and textures for bosses
    std::vector<Texture2D*> smokeTextures;

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
    std::list<EffectUpdateStrategy*> fgEffects;
    std::list<ESPEmitter*> bgEffectsEmitters;
    std::list<EffectUpdateStrategy*> laterPassEffects;

    virtual void DrawPreBodyEffects(double dT, const Camera& camera);
    virtual void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets) = 0;
    virtual void DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets);

    virtual Point3D GetBossFinalExplodingEpicenter() const = 0;

    ESPPointEmitter* BuildFireEmitter(float width, float height, float sizeScaler = 1.0f);
    ESPPointEmitter* BuildSmokeEmitter(float width, float height, float sizeScaler = 1.0f);
    ESPPointEmitter* BuildExplodingEmitter(float volumeAmt, const AbstractBossBodyPart* bossBodyPart, float width, float height, float sizeScaler = 1.0f);

    void BuildSummonPortalEffects(const Point3D& bossPos, const Point3D& portalPos, double effectTime,
        float size, const Colour& portalColour, std::list<ESPAbstractEmitter*>& emitters, 
        std::list<ESPPointToPointBeam*>& beams) const;

    static void BuildShieldingColourAnimation(AnimationMultiLerp<Colour>& anim);

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
    this->DrawPostBodyEffects(dT, camera, assets);
}

inline void BossMesh::DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets) {
    UNUSED_PARAMETER(assets);

    // Go through all the active post-body-effects for the boss, draw each one and clean up dead effects
    EffectUpdateStrategy::UpdateStrategyCollection(dT, camera, this->fgEffects);
}

inline void BossMesh::DrawLaterPassEffects(double dT, const Camera& camera) {

    // Go through all the active later-pass-effects for the boss, draw each one and clean up dead effects
    EffectUpdateStrategy::UpdateStrategyCollection(dT, camera, this->laterPassEffects);
}

#endif // __BOSSMESH_H__