#include "ESPParticleScaleEffector.h"
#include "ESPParticle.h"

ESPParticleScaleEffector::ESPParticleScaleEffector(float startScale, float endScale) :
startScale(startScale), endScale(endScale) {
}

ESPParticleScaleEffector::~ESPParticleScaleEffector() {
}

void ESPParticleScaleEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	// Perform linear interpolation on the scale of the particle
	// based on the particle's lifetime
	double newScale = this->startScale + particle->GetCurrentLifeElapsed() * 
		(this->endScale - this->startScale) / particle->GetLifespanLength();

	particle->MultiplyInitSizeScale(newScale);
}