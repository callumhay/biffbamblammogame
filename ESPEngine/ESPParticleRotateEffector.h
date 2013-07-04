/**
 * ESPParticleRotateEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

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
    bool useRotationSpd;

public:
	ESPParticleRotateEffector(float rotationSpd, RotationDirection dir);
	ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir);
	virtual ~ESPParticleRotateEffector(){}

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif