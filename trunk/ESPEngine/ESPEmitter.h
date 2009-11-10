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
class ESPEmitterEventHandler;

/**
 * ESPEmitter.h
 * Abstract base class representing an Emitter of particles.
 */
class ESPEmitter {
private:
	void TickParticles(double dT);

protected:			
	Texture2D* particleTexture;		// Texture of particles in this emitter

	std::map<const ESPParticle*, int> particleLivesLeft;		// Number of lives left for each particle
	std::list<ESPParticle*> aliveParticles;									// All the alive particles in this emitter
	std::list<ESPParticle*> deadParticles;									// All the dead particles in this emitter
	
	float timeSinceLastSpawn;																// Time since the last particle was spawned	
	int numParticleLives;
	
	bool isReversed;		// Whether this emitter is reversed (i.e., particles start where they die and die where they start)
	bool isPointSprite;	// Whether this emitter emits point sprites

	std::list<ESPParticleEffector*> effectors;					// All the particle effectors of this emitter
	std::list<ESPEmitterEventHandler*> eventHandlers;		// The event handlers attached to this emitter
	
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
	// Inclusive interval for how far from the emitPt this particle may spawn at
	ESPInterval radiusDeviationFromPt;	

	void Flush();

	virtual Vector3D CalculateRandomInitParticleDir() const = 0;
	virtual Point3D  CalculateRandomInitParticlePos() const = 0;
	void ReviveParticle();

public:
	ESPEmitter();
	virtual ~ESPEmitter();

	static const int ONLY_SPAWN_ONCE = -1;

	bool ParticlesHaveNoLivesLeft() const {
		if (this->numParticleLives != ESPParticle::INFINITE_PARTICLE_LIVES) {
			for (std::map<const ESPParticle*, int>::const_iterator iter = this->particleLivesLeft.begin(); iter != this->particleLivesLeft.end(); iter++) {
				if (iter->second != 0) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool IsDead() const {
		return (this->aliveParticles.size() == 0) && 
			     (this->timeSinceLastSpawn > this->particleLifetime.maxValue || this->ParticlesHaveNoLivesLeft());
	}

	bool OnlySpawnsOnce() const {
		return this->particleSpawnDelta.minValue == ESPEmitter::ONLY_SPAWN_ONCE;
	}

	// Setter functions for typical attributes of emitters
	bool SetParticles(unsigned int numParticles, Texture2D* texture);
	bool SetParticles(unsigned int numParticles, CgFxEffectBase* effect);
	bool SetParticles(unsigned int numParticles, const TextLabel2D& text);
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
	void SetRadiusDeviationFromCenter(const ESPInterval& distFromCenter);

	void AddEffector(ESPParticleEffector* effector);
	void RemoveEffector(ESPParticleEffector* const effector);
	void AddEventHandler(ESPEmitterEventHandler* eventHandler);

	void AddParticle(ESPParticle* particle);

	ESPInterval GetParticleSizeX() const { return this->particleSize[0]; }
	ESPInterval GetParticleSizeY() const { return this->particleSize[1]; }

	void Tick(double dT);
	void Draw(const Camera& camera, bool enableDepth = false);
	void Reset();

};
#endif