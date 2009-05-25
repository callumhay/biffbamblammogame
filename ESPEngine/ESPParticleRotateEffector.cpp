#include "ESPParticleRotateEffector.h"
#include "ESPParticle.h"

ESPParticleRotateEffector::ESPParticleRotateEffector(float rotationSpd, RotationDirection dir) : 
rotationSpd(rotationSpd), rotDir(dir), startRot(0), numRots(0) {
}

ESPParticleRotateEffector::ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir) :
startRot(initialRotation), numRots(numRotations), rotDir(dir), rotationSpd(0) {
}

void ESPParticleRotateEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	float particleLifespan = particle->GetLifespanLength();

	// Figure out how much to rotate based on the number of rotations before end of lifetime...
	// Only do this if we're dealing with a discrete lifetime
	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME) {

		float rotDegsPerSecond = this->rotDir * this->numRots * 360 / particleLifespan;	
		float currRotation = this->startRot + particle->GetCurrentLifeElapsed() * rotDegsPerSecond;

		particle->SetRotation(currRotation);
	}
	else {
		float currParticleRot = particle->GetRotation();
		particle->SetRotation(currParticleRot + this->rotDir*this->rotationSpd*dT);
	}
}