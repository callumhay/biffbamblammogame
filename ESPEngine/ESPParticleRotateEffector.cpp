/**
 * ESPParticleRotateEffector.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPParticleRotateEffector.h"
#include "ESPParticle.h"

ESPParticleRotateEffector::ESPParticleRotateEffector(float rotationSpd, RotationDirection dir) : 
rotationSpd(rotationSpd), rotDir(dir), startRot(0), numRots(0), useRotationSpd(true) {
}

ESPParticleRotateEffector::ESPParticleRotateEffector(float initialRotation, float numRotations, RotationDirection dir) :
startRot(initialRotation), numRots(numRotations), rotDir(dir), rotationSpd(0), useRotationSpd(false) {
}

void ESPParticleRotateEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	float particleLifespan = particle->GetLifespanLength();

	// Figure out how much to rotate based on the number of rotations before end of lifetime...
	// Do this if we're dealing with a discrete lifetime or if this rotate effector is specified to use the rotation speed
	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME && !this->useRotationSpd) {

		float rotDegsPerSecond = this->rotDir * this->numRots * 360 / particleLifespan;	
		float currRotation = this->startRot + particle->GetCurrentLifeElapsed() * rotDegsPerSecond;

		particle->SetRotation(currRotation);
	}
	else {
		float currParticleRot = particle->GetRotation();
		particle->SetRotation(currParticleRot + this->rotDir*this->rotationSpd*dT);
	}
}