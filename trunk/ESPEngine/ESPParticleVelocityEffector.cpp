#include "ESPParticleVelocityEffector.h"
#include "ESPParticle.h"

ESPParticleVelocityEffector::ESPParticleVelocityEffector(const Vector3D& velAmt) :
velocityAmount(velAmt) {
}

ESPParticleVelocityEffector::~ESPParticleVelocityEffector() {
}

void ESPParticleVelocityEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	// This Effector does not affect dead particles...
	if (particle->IsDead()) {
		return;
	}

	// Affect the particle by applying a constant velocity to it...
	particle->SetPosition(particle->GetPosition() + (dT * this->velocityAmount));
}