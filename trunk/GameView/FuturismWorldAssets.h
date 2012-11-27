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
    FuturismWorldAssets();
    ~FuturismWorldAssets();
    
    GameWorld::WorldStyle GetStyle() const;

    void Tick(double dT);
    void TickSkybeams(double dT);
	void DrawBackgroundEffects(const Camera& camera);
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

    void DrawFrontBeam(const Camera& camera, float rotationAmt);
    void DrawBackBeam(const Camera& camera, float rotationAmt);

private:

	Mesh* beamMesh;
	CgFxVolumetricEffect* backBeamEffect;
    CgFxVolumetricEffect* frontBeamEffect;

    // Rotations of each of the beams
    float rotBackLeftBeam1, rotBackLeftBeam2, rotBackLeftBeam3;
    float rotBackRightBeam1, rotBackRightBeam2, rotBackRightBeam3;
    float rotFrontLeftBeam1, rotFrontLeftBeam2, rotFrontLeftBeam3;
    float rotFrontRightBeam1, rotFrontRightBeam2, rotFrontRightBeam3;

    std::vector<AnimationMultiLerp<float>*> beamAnimators; 

    void InitializeTextures();
    void InitializeEmitters();
    void InitializeAnimations();

    DISALLOW_COPY_AND_ASSIGN(FuturismWorldAssets);
};

inline GameWorld::WorldStyle FuturismWorldAssets::GetStyle() const {
    return GameWorld::Futurism;
}

inline void FuturismWorldAssets::Tick(double dT) {
    GameWorldAssets::Tick(dT);
    this->TickSkybeams(dT);
}

inline void FuturismWorldAssets::DrawFrontBeam(const Camera& camera, float rotationAmt) {
    static const float BEAM_SCALE = 0.5f;
    glPushMatrix();
    glRotated(rotationAmt, 0, 0, 1);
    glScalef(BEAM_SCALE, 1.0f, BEAM_SCALE);
    this->beamMesh->Draw(camera, this->frontBeamEffect);
    glPopMatrix();
}

inline void FuturismWorldAssets::DrawBackBeam(const Camera& camera, float rotationAmt) {
    static const float BEAM_SCALE = 0.75f;
    glPushMatrix();
    glRotated(rotationAmt, 0, 0, 1);
    glScalef(BEAM_SCALE, 1.0f, BEAM_SCALE);
    this->beamMesh->Draw(camera, this->backBeamEffect);
    glPopMatrix();
}

#endif // __FUTURISMWORLDASSETS_H__