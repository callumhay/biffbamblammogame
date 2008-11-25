#include "ESPPointEmitter.h"


ESPPointEmitter::ESPPointEmitter() : ESPEmitter() {
}

ESPPointEmitter::~ESPPointEmitter() {
}

/**
 * Called each frame to execute the emitter.
 */
void ESPPointEmitter::Tick(const double dT) {
	
	// Go through each of the particles, revive any dead ones (based on spawn rate), and draw them
	for (std::vector<ESPParticle*>::iterator iter = this->particles.begin(); iter != this->particles.end(); iter++) {
		ESPParticle* currParticle = *iter;		
		
		// If the particle is dead revive it based on the spawn timer
		float allowableTimeToSpawn = this->particleSpawnDelta.RandomValueInInterval();
		if (currParticle->IsDead() && this->timeSinceLastSpawn >= allowableTimeToSpawn) {
			//TODO: currParticle->Revive(
			
			this->timeSinceLastSpawn = 0.0f;
		}

		// TODO...
	}

	// TODO...

	this->timeSinceLastSpawn += dT;
}