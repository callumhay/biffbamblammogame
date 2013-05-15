/**
 * GothicRomanticWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GOTHICROMANTICWORLDASSETS_H__
#define __GOTHICROMANTICWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameWorldAssets.h"
#include "CgFxVolumetricEffect.h"

#include "../ESPEngine/ESP.h"

class GothicRomanticWorldAssets : public GameWorldAssets {
public:
	GothicRomanticWorldAssets(GameAssets* assets);
	~GothicRomanticWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
        const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

private:
    Texture2D* cloudTex;

    CgFxVolumetricEffect fireEffect;
    ESPPointEmitter fireEmitter1, fireEmitter2, fireEmitter4, fireEmitter5, fireEmitter6;
    ESPParticleAccelEffector fireAccel1, fireAccel2, fireAccel4, fireAccel5, fireAccel6;

    ESPParticleColourEffector fireColourFader;
    ESPParticleScaleEffector fireParticleScaler;

    void InitializeEmitters();
    void ApplyRandomFireAccel(ESPParticleAccelEffector& accelEffector);

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

inline void GothicRomanticWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    this->fireEmitter1.Draw(camera);
    this->fireEmitter2.Draw(camera);
    this->fireEmitter4.Draw(camera);
    this->fireEmitter5.Draw(camera);
    this->fireEmitter6.Draw(camera);
}

#endif // __GOTHICROMANTICWORLDASSETS_H__