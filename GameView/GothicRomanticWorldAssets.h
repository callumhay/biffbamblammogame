/**
 * GothicRomanticWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
	GothicRomanticWorldAssets();
	~GothicRomanticWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
        const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

private:
    
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
    UNUSED_PARAMETER(camera);
}

inline void GothicRomanticWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);
}

#endif // __GOTHICROMANTICWORLDASSETS_H__