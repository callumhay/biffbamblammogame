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