/**
 * DecoWorldAssets.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __DECOWORLDASSETS_H__
#define __DECOWORLDASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "GameWorldAssets.h"

#include "../ESPEngine/ESPVolumeEmitter.h"
#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"
#include "../ESPEngine/ESPParticleRotateEffector.h"

class CgFxVolumetricEffect;

class DecoWorldAssets : public GameWorldAssets {
public:
	DecoWorldAssets();
	~DecoWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight, const BasicPointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

private:

	enum RotationState { RotateCW, RotateCCW };		// CCW is moving in postive degrees, CW is negative...

	double rotationLimitfg, rotationLimitbg;		// Limit of the arc of rotation for the beams
	double rotationSpd;								// Rotation speed of beams in degrees/second
	
	// Current rotation amount of each beam and state
	double beamRotationfg1, beamRotationfg2, beamRotationbg1, beamRotationbg2;				
	RotationState rotationStatefg1, rotationStatefg2, rotationStatebg1, rotationStatebg2;

	// Background meshes and effects specific to deco world
	Mesh* skybeam;
	CgFxVolumetricEffect* beamEffect;

	Texture2D* spiralTexSm;
	Texture2D* spiralTexMed;
	Texture2D* spiralTexLg;
	ESPVolumeEmitter spiralEmitterSm, spiralEmitterMed, spiralEmitterLg;
	ESPParticleRotateEffector rotateEffectorCW, rotateEffectorCCW;
	
	void InitializeEmitters();
	void RotateSkybeams(double dT);

    DISALLOW_COPY_AND_ASSIGN(DecoWorldAssets);
};

inline GameWorld::WorldStyle DecoWorldAssets::GetStyle() const {
	return GameWorld::Deco;
}

#endif