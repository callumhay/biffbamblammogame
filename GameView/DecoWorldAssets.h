#ifndef __DECOWORLDASSETS_H__
#define __DECOWORLDASSETS_H__

#include "GameWorldAssets.h"

class CgFxVolumetricEffect;

class DecoWorldAssets : public GameWorldAssets {
private:
	enum RotationState {RotateCW, RotateCCW};		// CCW is moving in postive degrees, CW is negative...

	double rotationLimitfg, rotationLimitbg;		// Limit of the arc of rotation for the beams
	double rotationSpd;													// Rotation speed of beams in degrees/second
	
	// Current rotation amount of each beam and state
	double beamRotationfg1, beamRotationfg2, beamRotationbg1, beamRotationbg2;				
	RotationState rotationStatefg1, rotationStatefg2, rotationStatebg1, rotationStatebg2;

	Mesh* skybeam;
	CgFxVolumetricEffect* beamEffect;
	
	void RotateSkybeams(double dT);

public:
	DecoWorldAssets();
	virtual ~DecoWorldAssets();

	virtual void DrawBackground(double dT, const Camera& camera);

};
#endif