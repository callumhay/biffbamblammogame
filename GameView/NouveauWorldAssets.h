/**
 * NouveauWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __NOUVEAUWORLDASSETS_H__
#define __NOUVEAUWORLDASSETS_H__

#include "../BlammoEngine/Bezier.h"
#include "../ESPEngine/ESPVolumeEmitter.h"
#include "../ESPEngine/ESPMultiColourEffector.h"
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

	void Tick(double dT);

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