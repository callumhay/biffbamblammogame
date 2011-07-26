/**
 * ESPParticleAccelEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPPARTICLEACCELEFFECTOR_H__
#define __ESPPARTICLEACCELEFFECTOR_H__

#include "../BlammoEngine/Vector.h"

#include "ESPParticleEffector.h"

class ESPParticleAccelEffector : public ESPParticleEffector {

private:
	// The amount of acceleration in m/s^2 to apply to the particle it affects
	Vector3D accelAmount;

public:
	ESPParticleAccelEffector(const Vector3D& accelAmount);
	virtual ~ESPParticleAccelEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);

	void SetAcceleration(const Vector3D& a) {
		this->accelAmount = a;
	}
};
#endif