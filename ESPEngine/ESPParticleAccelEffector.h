#ifndef __ESPPARTICLEACCELEFFECTOR_H__
#define __ESPPARTICLEACCELEFFECTOR_H__

#include "../BlammoEngine/Vector.h"

#include "ESPParticleEffector.h"

class ESPParticleAccelEffector : public ESPParticleEffector {

private:
	// The amount of velocity in m/s to apply to the particle it affects
	Vector3D accelAmount;

public:
	ESPParticleAccelEffector(const Vector3D& accelAmount);
	virtual ~ESPParticleAccelEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif