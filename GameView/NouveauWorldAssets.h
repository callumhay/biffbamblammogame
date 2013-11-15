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