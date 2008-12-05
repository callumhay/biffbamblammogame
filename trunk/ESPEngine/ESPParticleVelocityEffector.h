#ifndef __ESPPARTICLEVELOCITYEFFECTOR_H__
#define __ESPPARTICLEVELOCITYEFFECTOR_H__

#include "../BlammoEngine/Vector.h"

#include "ESPParticleEffector.h"

class ESPParticleVelocityEffector : public ESPParticleEffector {

private:
	// The amount of velocity in m/s to apply to the particle it affects
	Vector3D velocityAmount;

public:
	ESPParticleVelocityEffector(const Vector3D& velAmt);
	virtual ~ESPParticleVelocityEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif