/**
 * NouveauWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __NOUVEAUWORLDASSETS_H__
#define __NOUVEAUWORLDASSETS_H__

#include "GameWorldAssets.h"

class NouveauWorldAssets : public GameWorldAssets {
public:
	NouveauWorldAssets();
	~NouveauWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

private:

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

#endif // __NOUVEAUWORLDASSETS_H__