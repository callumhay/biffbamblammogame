/**
 * FuturismWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __FUTURISMWORLDASSETS_H__
#define __FUTURISMWORLDASSETS_H__

#include "GameWorldAssets.h"

class FuturismWorldAssets : public GameWorldAssets {
public:
    FuturismWorldAssets();
    ~FuturismWorldAssets();
    
    GameWorld::WorldStyle GetStyle() const;

    void Tick(double dT);
	void DrawBackgroundEffects(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

private:
    DISALLOW_COPY_AND_ASSIGN(FuturismWorldAssets);
};

inline GameWorld::WorldStyle FuturismWorldAssets::GetStyle() const {
    return GameWorld::Futurism;
}

#endif // __FUTURISMWORLDASSETS_H__