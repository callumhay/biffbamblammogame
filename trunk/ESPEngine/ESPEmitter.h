#ifndef __ESPEMITTER_H__
#define __ESPEMITTER_H__

#include "../BlammoEngine/Camera.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPParticle.h"
#include "ESPUtil.h"
#include "ESPParticleEffector.h"
#include "ESPParticleBatchMesh.h"

#include <vector>
#include <string>
#include <map>

class CgFxEffectBase;
class TextLabel2D;

/**
 * ESPEmitter.h
 * Abstract base class representing an Emitter of particles.
 */
class ESPEmitter {

protected:			
	Texture2D* particleTexture;		// Texture of particles in this emitter

	std::map<const ESPParticle*, int> particleLivesLeft;		// Number of lives left for each particle
	std::list<ESPParticle*> aliveParticles;									// All the alive particles in this emitter
	std::list<ESPParticle*> deadParticles;									// All the dead particles in this emitter
	
	float timeSinceLastSpawn;																// Time since the last particle was spawned	
	int numParticleLives;
	
	bool isReversed;		// Whether this emitter is reversed (i.e., particles start where they die and die where they start)
	bool isPointSprite;	// Whether this emitter emits point sprites

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

	void Flush();

public:
	ESPEmitter();
	virtual ~ESPEmitter();

	static const int ONLY_SPAWN_ONCE = -1;

	bool IsDead() const {
		return (this->aliveParticles.size() == 0) && 
			     (this->timeSinceLastSpawn > this->particleLifetime.maxValue);
	}

	bool OnlySpawnsOnce() const {
		return this->particleSpawnDelta.minValue == ESPEmitter::ONLY_SPAWN_ONCE;
	}

	// Setter functions for typical attributes of emitters
	bool SetParticles(unsigned int numParticles, Texture2D* texture);
	bool SetParticles(unsigned int numParticles, CgFxEffectBase* effect);
	bool SetParticles(unsigned int numParticles, const TextLabel2D& text, Onomatoplex::SoundType st, Onomatoplex::Extremeness e);

	void SetParticleAlignment(const ESP::ESPAlignment alignment);
	void SetSpawnDelta(const ESPInterval& spawnDelta);
	void SetInitialSpd(const ESPInterval& initialSpd);
	void SetParticleLife(const ESPInterval& particleLife);
	void SetParticleSize(const ESPInterval& particleSizeX, const ESPInterval& particleSizeY);
	void SetParticleSize(const ESPInterval& particleSize);
	void SetParticleColour(const ESPInterval& red, const ESPInterval& green, const ESPInterval& blue, const ESPInterval& alpha);
	void SetParticleRotation(const ESPInterval& particleRot);
	void SetNumParticleLives(int lives);
	void SetIsReversed(bool isReversed);
	void SetAsPointSpriteEmitter(bool isPointSprite);

	void AddEffector(ESPParticleEffector* effector);
	void RemoveEffector(ESPParticleEffector* const effector);
	void AddParticle(ESPParticle* particle);


	ESPInterval GetParticleSizeX() const { return this->particleSize[0]; }
	ESPInterval GetParticleSizeY() const { return this->particleSize[1]; }

	virtual void Tick(double dT) = 0;
	virtual void Draw(const Camera& camera) = 0;
	virtual void Reset();

};
#endif