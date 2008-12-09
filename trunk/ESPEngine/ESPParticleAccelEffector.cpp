#include "ESPParticleAccelEffector.h"
#include "ESPParticle.h"

ESPParticleAccelEffector::ESPParticleAccelEffector(const Vector3D& accelAmount) :
accelAmount(accelAmount) {
}

ESPParticleAccelEffector::~ESPParticleAccelEffector() {
}

void ESPParticleAccelEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	// This Effector does not affect dead particles...
	if (particle->IsDead()) {
		return;
	}

	// Affect the particle by applying a constant velocity to it...
	particle->SetVelocity(particle->GetVelocity() + (dT * this->accelAmount));
}