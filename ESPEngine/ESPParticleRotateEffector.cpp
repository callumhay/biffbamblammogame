#include "ESPParticleRotateEffector.h"
#include "ESPParticle.h"

ESPParticleRotateEffector::ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir) :
startRot(initialRotation), numRots(numRotations), rotDir(dir) {
}

void ESPParticleRotateEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	float particleLifespan = particle->GetLifespanLength();

	// Figure out how much to rotate based on the number of rotations before end of lifetime...
	// Only do this if we're dealing with a discrete lifetime
	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME) {

		float rotDegsPerSecond = rotDir * this->numRots * 360 / particleLifespan;	
		float currRotation = this->startRot + particle->GetCurrentLifeElapsed() * rotDegsPerSecond;

		particle->SetRotation(currRotation);
	}
}