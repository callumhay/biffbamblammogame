#ifndef __ESPPARTICLEVELOCITYEFFECTOR_H__
#define __ESPPARTICLEVELOCITYEFFECTOR_H__

#include "ESPParticleEffector.h"

class ESPParticleVelocityEffector : public ESPParticleEffector {

private:

	// The amount of velocity in m/s to apply to the particle it affects
	float velocityAmount;
	

public:
	ESPParticleVelocityEffector();
	virtual ~ESPParticleVelocityEffector();

	virtual void EffectParticle(ESPParticle* particle);
};
#endif