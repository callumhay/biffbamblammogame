/**
 * GothicRomanticBossMesh.h
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

#ifndef __GOTHICROMANTICBOSSMESH_H__
#define __GOTHICROMANTICBOSSMESH_H__

#include "BossMesh.h"

class GothicRomanticBoss;
class Mesh;
class ESPPointEmitter;

class GothicRomanticBossMesh : public BossMesh {
public:
    GothicRomanticBossMesh(GothicRomanticBoss* boss, GameSound* sound);
    ~GothicRomanticBossMesh();

    // Inherited from BossMesh
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

    SoundID bottomGlowSoundID;
    std::vector<SoundID> legGlowSoundIDs;

    // Inherited from BossMesh;
    void DrawBody(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, const GameAssets* assets);
    void DrawPostBodyEffects(double dT, const Camera& camera, const GameAssets* assets);

    Point3D GetBossFinalExplodingEpicenter() const;

    DISALLOW_COPY_AND_ASSIGN(GothicRomanticBossMesh);
};

#endif // __GOTHICROMANTICBOSSMESH_H__