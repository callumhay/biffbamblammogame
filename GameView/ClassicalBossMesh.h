/**
 * ClassicalBossMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __CLASSICALBOSSMESH_H__
#define __CLASSICALBOSSMESH_H__

#include "BossMesh.h"

#include "../BlammoEngine/Light.h"

class ClassicalBoss;
class Mesh;
class ESPPointEmitter;

class ClassicalBossMesh : public BossMesh {
public:
    ClassicalBossMesh(ClassicalBoss* boss, GameSound* sound);
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

    SoundID sparkleSoundID;
    SoundID glowSoundID;

    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);
    void DrawPostBodyEffects(double dT, const Camera& camera, const GameModel& gameModel, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
        const BasicPointLight& ballLight,const GameAssets* assets);

    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(ClassicalBossMesh);
};

#endif // __CLASSICALBOSSMESH_H__