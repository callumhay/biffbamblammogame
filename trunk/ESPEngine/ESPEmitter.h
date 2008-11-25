#ifndef __ESPEMITTER_H__
#define __ESPEMITTER_H__

#include "ESPParticle.h"
#include "ESPUtil.h"
#include "ESPParticleEffector.h"

#include <vector>
#include <string>

/**
 * ESPEmitter.h
 * Abstract base class representing an Emitter of particles.
 */
class ESPEmitter {

protected:
	
	Texture2D* particleTexture;										// Texture of particles in this emitter
	std::vector<ESPParticle*> particles;					// All the particles making up this emitter
	std::vector<ESPParticleEffector*> effectors;	// All the particle effectors of this emitter
	float timeSinceLastSpawn;

	// Inclusive interval of time between firing/spawning of particles in seconds
	ESPInterval particleSpawnDelta;
	// Inclusive interval for initial Speed of particles
	ESPInterval particleInitialSpd;
	// Inclusive interval for lifetime (in seconds) of particles
	ESPInterval particleLifetime;
	// Inclusive interval for size of particles (in units)
	ESPInterval particleSize;

	virtual void Flush();

public:
	ESPEmitter();
	virtual ~ESPEmitter();

	// Setter functions for typical attributes of emitters
	bool SetParticles(unsigned int numParticles, const std::string& imgFilepath);
	void SetSpawnDelta(const ESPInterval& spawnDelta);
	void SetInitialSpd(const ESPInterval& initialSpd);
	void SetParticleLife(const ESPInterval& particleLife);
	void SetParticleSize(const ESPInterval& particleSize);
	
	virtual void Tick(const double dT) = 0;
	
};
#endif