/**
 * ESPEmitterEventHandler.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef _ESPEMITTEREVENTHANDLER_H_
#define _ESPEMITTEREVENTHANDLER_H_

class ESPParticle;

/**
 * Event handler that can be attached to ESPEmitter objects.
 * This handler will signal various events as they happen within the emitter.
 */
class ESPEmitterEventHandler {
public:
	/**
	 * Event that occurs when a particle is spawned within the associated emitter.
	 * particle - The particle that was emitted.
	 */
	virtual void ParticleSpawnedEvent(const ESPParticle* particle) = 0;

};
#endif // _ESPEMITTEREVENTHANDLER_H_