/**
 * FuturismBossMesh.h
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

#ifndef __FUTURISMBOSSMESH_H__
#define __FUTURISMBOSSMESH_H__

#include "BossMesh.h"
#include "CgFxVolumetricEffect.h"

class FuturismBoss;
class Mesh;

class FuturismBossMesh : public BossMesh {
public:
    FuturismBossMesh(FuturismBoss* boss, GameSound* sound);
    ~FuturismBossMesh();

    // Inherited from BossMesh
    double ActivateIntroAnimation();

private:
    static const double INTRO_TIME_IN_SECS;

    FuturismBoss* boss;  // N.B., The pointed-to object doesn't belong to this

    double introTimeCountdown;

    // Meshes for all of the parts of the boss' structure/body
    Mesh* coreCenterMesh;
    Mesh* coreArmsMesh;
    Mesh* coreBulbMesh;
    Mesh* coreShieldMesh;
    Mesh* leftRightShieldMesh;
    Mesh* topBottomShieldMesh;

    Mesh* damagedLeftRightShieldMesh;
    Mesh* damagedTopBottomShieldMesh;
    Mesh* damagedCoreShieldMesh;

    Mesh* iceEncasingMesh;

    // Effect assets
    AnimationMultiLerp<Colour> shieldColourAnim;
    AnimationMultiLerp<Colour> bulbFlashingAnim;

    ESPPointEmitter* topShieldExplodingEmitter;
    ESPPointEmitter* bottomShieldExplodingEmitter;
    ESPPointEmitter* leftShieldExplodingEmitter;
    ESPPointEmitter* rightShieldExplodingEmitter;
    ESPPointEmitter* coreShieldExplodingEmitter;
    ESPPointEmitter* coreExplodingEmitter;

    ESPPointEmitter eyePulseGlow;
    ESPPointEmitter frostMist;
    CgFxVolumetricEffect frostMistShader;

    SoundID glowSoundID;

    // Inherited functions
    //void DrawPreBodyEffects(double dT, const Camera& camera, const GameModel& gameModel);
    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);
    void DrawPostBodyEffects(double dT, const Camera& camera, const GameModel& gameModel, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
        const BasicPointLight& ballLight,const GameAssets* assets);
    Point3D GetBossFinalExplodingEpicenter() const;

    // Helper functions
    void DrawDamagableBodyPart(const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight,
        const BossBodyPart* bodyPart, Mesh* nonDmgMesh, Mesh * dmgMesh) const;
    void DrawBulbBodyPart(const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight,
        const BossBodyPart* bodyPart, Mesh* mesh) const;
    void DrawShieldPostEffects(double dT, const Camera& camera, float xOffset, float yOffset,
        const BossBodyPart* shield, ESPPointEmitter* explodingEmitter);

    DISALLOW_COPY_AND_ASSIGN(FuturismBossMesh);
};
#endif // __FUTURISMBOSSMESH_H__