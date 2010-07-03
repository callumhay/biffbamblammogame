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

private:
	static const float COLOUR_CHANGE_TIME;
	static const int NUM_COLOUR_CHANGES = 10;
	static const Colour COLOUR_CHANGE_LIST[NUM_COLOUR_CHANGES];

	AnimationMultiLerp<Colour> currBGMeshColourAnim;	// Colour animation progression of the background mesh
	AnimationMultiLerp<float> bgFadeAnim;							// Fade animation (for the alpha) for when the background is being fadeded in/out

	enum RotationState { RotateCW, RotateCCW };		// CCW is moving in postive degrees, CW is negative...

	double rotationLimitfg, rotationLimitbg;		// Limit of the arc of rotation for the beams
	double rotationSpd;													// Rotation speed of beams in degrees/second
	
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

public:
	DecoWorldAssets();
	~DecoWorldAssets();

	GameWorld::WorldStyle GetStyle() const;
	void DrawBackgroundModel(const Camera& camera, const PointLight& bgKeyLight, const PointLight& bgFillLight);
	void DrawBackgroundEffects(const Camera& camera);
	void FadeBackground(bool fadeout, float fadeTime);
	void ResetToInitialState();

	void Tick(double dT);

};

inline GameWorld::WorldStyle DecoWorldAssets::GetStyle() const {
	return GameWorld::Deco;
}

#endif