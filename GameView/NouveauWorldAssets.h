/**
 * NouveauWorldAssets.h
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

#ifndef __NOUVEAUWORLDASSETS_H__
#define __NOUVEAUWORLDASSETS_H__

#include "../BlammoEngine/Bezier.h"
#include "../ESPEngine/ESP.h"
#include "GameWorldAssets.h"

class ESPVolumeEmitter;

class NouveauWorldAssets : public GameWorldAssets {
public:
	NouveauWorldAssets(GameAssets* assets);
	~NouveauWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT, const GameModel& model);

    void LoadFGLighting(GameAssets* assets, const Vector3D& fgKeyPosOffset = Vector3D(0,0,0),
        const Vector3D& fgFillPosOffset = Vector3D(0,0,0)) const;
    void LoadBGLighting(GameAssets* assets) const;

private:
    /*
    // Background effects: Nouveau curves/flourishes
    std::vector<Bezier*> curves;
    ESPVolumeEmitter leftSideEmitter;
    ESPVolumeEmitter centerEmitter;
    ESPVolumeEmitter rightSideEmitter;
    ESPVolumeEmitter leftGapEmitter;
    ESPVolumeEmitter rightGapEmitter;
    ESPMultiColourEffector fadeEffector;
    */

    const Point3D lamp1Pos;
    const Point3D lamp2Pos;

    static const float HALO_ALPHA_MULTIPLIER;
    static const float LENS_FLARE_ALPHA_MULTIPLIER;

    Texture2D* glowTex;
    Texture2D* haloTex;
    Texture2D* lensFlareTex;

    ESPPointEmitter lamp1GlowEmitter;
    ESPPointEmitter lamp1HaloEmitter;
    ESPPointEmitter lamp1LensFlareEmitter;
    ESPPointEmitter lamp2GlowEmitter;
    ESPPointEmitter lamp2HaloEmitter;
    ESPPointEmitter lamp2LensFlareEmitter;

    ESPParticleScaleEffector glowPulse;
    ESPParticleScaleEffector haloPulse;
    //Texture2D* haloTex;


    void InitEmitters();
    void BuildLampEmitter(const Point3D& pos, ESPPointEmitter& glowEmitter, 
        ESPPointEmitter& haloEmitter, ESPPointEmitter& lensFlareEmitter);

    DISALLOW_COPY_AND_ASSIGN(NouveauWorldAssets);
};

inline GameWorld::WorldStyle NouveauWorldAssets::GetStyle() const {
	return GameWorld::Nouveau;
}

/**
 * Fade the background out or in based on the given parameter, the fade
 * will occur over the given amount of time.
 */
inline void NouveauWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

/**
 * Reset the world to its initial state - makes sure that nothing is faded/invisible etc.
 * and is ready to be see for the first time by the player.
 */
inline void NouveauWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();
}

#endif // __NOUVEAUWORLDASSETS_H__