#ifndef __ESPPARTICLEEFFECTOR_H__
#define __ESPPARTICLEEFFECTOR_H__

class ESPParticle;

/**
 * ESPParticleEffector.h
 * Abstract class used for making effectors for particles (e.g., force, noise).
 */
class ESPParticleEffector {

public:
	ESPParticleEffector(){};
	virtual ~ESPParticleEffector(){};

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle) = 0;

};
#endif