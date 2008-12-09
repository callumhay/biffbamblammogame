#ifndef __ESPEMITTER_H__
#define __ESPEMITTER_H__

#include "../BlammoEngine/Camera.h"

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
	std::list<ESPParticle*> aliveParticles;				// All the alive particles in this emitter
	std::list<ESPParticle*> deadParticles;				// All the dead particles in this emitter
	float timeSinceLastSpawn;											// Time since the last particle was spawned	

	std::list<ESPParticleEffector*> effectors;	// All the particle effectors of this emitter
	
	// The alignment of particles in this emitter w.r.t. the viewer
	ESP::ESPAlignment particleAlignment;
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
	void SetParticleAlignment(const ESP::ESPAlignment alignment);
	void SetSpawnDelta(const ESPInterval& spawnDelta);
	void SetInitialSpd(const ESPInterval& initialSpd);
	void SetParticleLife(const ESPInterval& particleLife);
	void SetParticleSize(const ESPInterval& particleSize);
	
	void AddEffector(ESPParticleEffector* effector);
	void RemoveEffector(ESPParticleEffector* const effector);
	void AddParticle(ESPParticle* particle);

	virtual void Tick(const double dT) = 0;
	virtual void Draw(const Camera& camera) = 0;
	
};
#endif