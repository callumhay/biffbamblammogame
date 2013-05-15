/**
 * ESPParticleMagneticEffector.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "ESPParticleMagneticEffector.h"
#include "ESPParticle.h"

ESPParticleMagneticEffector::ESPParticleMagneticEffector(const Point3D& attactingPolePos, float forceCoeff) :
attactingPolePos(attactingPolePos), forceCoeff(forceCoeff) {
}

ESPParticleMagneticEffector::~ESPParticleMagneticEffector() {
}

void ESPParticleMagneticEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	// This Effector does not affect dead particles...
	if (particle->IsDead()) {
		return;
	}

    // Totally fudging magnetic force here, we use the inverse of the squared distance
    // to diminish the amount of force applied to the particle...
    const Point3D& particlePos = particle->GetPosition();
    Vector3D particleToPoleVec = (this->attactingPolePos - particlePos);
    float squareDistanceToPole = particleToPoleVec.length2();
    if (squareDistanceToPole < 0.01) {
        particle->SetVelocity(Vector3D(0,0,0));
        return;
    }

    particleToPoleVec.Normalize();

	// Affect the particle's velocity by accelerating by the calculated force
	particle->SetVelocity(particle->GetVelocity() + ((dT * this->forceCoeff / squareDistanceToPole) * particleToPoleVec));
}