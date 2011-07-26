/**
 * ESPParticleEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

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