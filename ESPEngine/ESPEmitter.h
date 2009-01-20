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
	ESPInterval particleSize[2];
	bool makeSizeConstraintsEqual;
	// Inclusive interval for the rotation of particles (in degrees)
	ESPInterval particleRotation;
	// Inclusive intervals for the colour of particles
	ESPInterval particleRed, particleGreen, particleBlue, particleAlpha;

	virtual void Flush();

public:
	ESPEmitter();
	virtual ~ESPEmitter();

	bool IsDead() const {
		return (this->aliveParticles.size() == 0) && 
			     (this->timeSinceLastSpawn > this->particleLifetime.maxValue);
	}

	// Setter functions for typical attributes of emitters
	bool SetParticles(unsigned int numParticles, Texture2D* texture);
	void SetParticleAlignment(const ESP::ESPAlignment alignment);
	void SetSpawnDelta(const ESPInterval& spawnDelta);
	void SetInitialSpd(const ESPInterval& initialSpd);
	void SetParticleLife(const ESPInterval& particleLife);
	void SetParticleSize(const ESPInterval& particleSizeX, const ESPInterval& particleSizeY);
	void SetParticleSize(const ESPInterval& particleSize);
	void SetParticleColour(const ESPInterval& red, const ESPInterval& green, const ESPInterval& blue, const ESPInterval& alpha);
	void SetParticleRotation(const ESPInterval& particleRot);
	
	void AddEffector(ESPParticleEffector* effector);
	void RemoveEffector(ESPParticleEffector* const effector);
	void AddParticle(ESPParticle* particle);

	virtual void Tick(double dT) = 0;
	virtual void Draw(const Camera& camera) = 0;
	virtual void Reset();

};
#endif