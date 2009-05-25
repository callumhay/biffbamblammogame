#ifndef __ESPPARTICLEROTATEEFFECTOR_H__
#define __ESPPARTICLEROTATEEFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleRotateEffector : public ESPParticleEffector {
public:
	enum RotationDirection { CLOCKWISE = 1, COUNTER_CLOCKWISE = -1 };

private:
	float rotationSpd;
	float startRot, numRots;
	RotationDirection rotDir;

public:
	ESPParticleRotateEffector(float rotationSpd, RotationDirection dir);
	ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir);
	virtual ~ESPParticleRotateEffector(){}

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif