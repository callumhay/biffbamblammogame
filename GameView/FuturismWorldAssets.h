/**
 * FuturismWorldAssets.h
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

#ifndef __FUTURISMWORLDASSETS_H__
#define __FUTURISMWORLDASSETS_H__

#include "GameWorldAssets.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "../ESPEngine/ESPVolumeEmitter.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleRotateEffector.h"

#include "CgFxVolumetricEffect.h"

class FuturismWorldAssets : public GameWorldAssets {
public:
    FuturismWorldAssets(GameAssets* assets);
    ~FuturismWorldAssets();
    
    GameWorld::WorldStyle GetStyle() const;

    void Tick(double dT, const GameModel& model);
    void TickSkybeams(double dT);
	void DrawBackgroundEffects(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

    //void DrawFrontBeam(const Camera& camera, float rotationAmt);
    void DrawBackBeam(const Camera& camera, float rotationAmt);

    void LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset = Vector3D(0,0,0), 
        const Vector3D& fgFillPosOffset = Vector3D(0,0,0)) const;
    void LoadBGLighting(GameAssets* assets) const;

private:

	Mesh* beamMesh;
	CgFxVolumetricEffect backBeamEffect;
    //CgFxVolumetricEffect frontBeamEffect;

    // Rotations of each of the beams
    float rotBackLeftBeam1, rotBackLeftBeam2, rotBackLeftBeam3;
    float rotBackRightBeam1, rotBackRightBeam2, rotBackRightBeam3;
    //float rotFrontLeftBeam1, rotFrontLeftBeam2, rotFrontLeftBeam3;
    //float rotFrontRightBeam1, rotFrontRightBeam2, rotFrontRightBeam3;

    std::vector<AnimationMultiLerp<float>*> beamAnimators; 

    Texture2D* futurismTriangleTex;
    ESPVolumeEmitter triangleEmitterSm, triangleEmitterMed, triangleEmitterLg;
    ESPParticleColourEffector triangleFader;

    void InitializeTextures();
    void InitializeEmitters();
    void InitializeAnimations();

    DISALLOW_COPY_AND_ASSIGN(FuturismWorldAssets);
};

inline GameWorld::WorldStyle FuturismWorldAssets::GetStyle() const {
    return GameWorld::Futurism;
}

inline void FuturismWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

inline void FuturismWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();
}

inline void FuturismWorldAssets::Tick(double dT, const GameModel& model) {
    this->TickSkybeams(dT);

    float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();

    this->triangleEmitterSm.Tick(dT);
    this->triangleEmitterSm.SetAliveParticleAlphaMax(currBGAlpha);
    this->triangleEmitterMed.Tick(dT);
    this->triangleEmitterMed.SetAliveParticleAlphaMax(currBGAlpha);
    this->triangleEmitterLg.Tick(dT);
    this->triangleEmitterLg.SetAliveParticleAlphaMax(currBGAlpha);

    GameWorldAssets::Tick(dT, model);
}

//inline void FuturismWorldAssets::DrawFrontBeam(const Camera& camera, float rotationAmt) {
//    static const float BEAM_SCALE = 0.5f;
//    glPushMatrix();
//    glRotated(rotationAmt, 0, 0, 1);
//    glScalef(BEAM_SCALE, 1.0f, BEAM_SCALE);
//    this->beamMesh->Draw(camera, this->frontBeamEffect);
//    glPopMatrix();
//}

inline void FuturismWorldAssets::DrawBackBeam(const Camera& camera, float rotationAmt) {
    static const float BEAM_SCALE = 0.75f;
    glPushMatrix();
    glRotated(rotationAmt, 0, 0, 1);
    glScalef(BEAM_SCALE, 1.0f, BEAM_SCALE);
    this->beamMesh->Draw(camera, &this->backBeamEffect);
    glPopMatrix();
}

#endif // __FUTURISMWORLDASSETS_H__