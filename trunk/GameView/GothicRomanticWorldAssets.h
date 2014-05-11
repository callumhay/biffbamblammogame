/**
 * GothicRomanticWorldAssets.h
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

#ifndef __GOTHICROMANTICWORLDASSETS_H__
#define __GOTHICROMANTICWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameWorldAssets.h"
#include "CgFxVolumetricEffect.h"
#include "CgFxCloudEffect.h"

#include "../ESPEngine/ESP.h"

class GothicRomanticWorldAssets : public GameWorldAssets {
public:
	GothicRomanticWorldAssets(GameAssets* assets);
	~GothicRomanticWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
        const BasicPointLight& bgFillLight);
    void DrawBackgroundPostOutlinePreEffects(const Camera& camera);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT, const GameModel& model);

private:
    static const float MOON_CLOUD_ALPHA;
    static const float LESS_VISIBLE_CLOUD_ALPHA;

    const Point3D moonPos;
    std::vector<Texture2D*> cloudTextures;
    Texture2D* moonTex;

    CgFxVolumetricEffect fireEffect;
    CgFxCloudEffect cloudEffect;

    ESPPointEmitter fireEmitter1, fireEmitter2, fireEmitter4, fireEmitter5, fireEmitter6;
    ESPParticleAccelEffector fireAccel1, fireAccel2, fireAccel4, fireAccel5, fireAccel6;

    ESPParticleColourEffector fireColourFader;
    ESPParticleScaleEffector fireParticleScaler;

    ESPVolumeEmitter moonCloudEmitter;
    ESPMultiColourEffector moonCloudFader;
    ESPParticleScaleEffector cloudGrower;

    ESPVolumeEmitter lowerCloudEmitter;
    ESPMultiColourEffector lessVisibleCloudFader;

    void LoadBGLighting(GameAssets* assets) const;

    void InitializeEmitters();
    void ApplyRandomFireAccel(ESPParticleAccelEffector& accelEffector);

    void BuildCloudEmitter(const Point3D& min, const Point3D& max, int dir, ESPMultiColourEffector& colourEffector, ESPVolumeEmitter& emitter);
    void BuildFrontDoorFireEmitter(const Point3D& pos, ESPPointEmitter& emitter);
    void BuildWindowWallFireEmitter(const Point3D& pos, ESPPointEmitter& emitter);
    

    DISALLOW_COPY_AND_ASSIGN(GothicRomanticWorldAssets);
};

inline GameWorld::WorldStyle GothicRomanticWorldAssets::GetStyle() const {
    return GameWorld::GothicRomantic;
}

inline void GothicRomanticWorldAssets::FadeBackground(bool fadeout, float fadeTime) {
    GameWorldAssets::FadeBackground(fadeout, fadeTime);
}

inline void GothicRomanticWorldAssets::ResetToInitialState() {
    GameWorldAssets::ResetToInitialState();
}

#endif // __GOTHICROMANTICWORLDASSETS_H__