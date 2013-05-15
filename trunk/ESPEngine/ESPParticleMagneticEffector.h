/**
 * ESPParticleMagneticEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPPARTICLEMAGNETICEFFECTOR_H__
#define __ESPPARTICLEMAGNETICEFFECTOR_H__

#include "ESPParticleEffector.h"
#include "../BlammoEngine/Point.h"

class ESPParticleMagneticEffector : public ESPParticleEffector {

public:
	ESPParticleMagneticEffector(const Point3D& attactingPolePos, float forceCoeff);
	~ESPParticleMagneticEffector();

	void AffectParticleOnTick(double dT, ESPParticle* particle);

private:
    Point3D attactingPolePos;
    float forceCoeff;

};

#endif // __ESPPARTICLEMAGNETICEFFECTOR_H__