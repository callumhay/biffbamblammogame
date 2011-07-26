/**
 * ESPParticleScaleEffector.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPParticleScaleEffector.h"
#include "ESPParticle.h"

ESPParticleScaleEffector::ESPParticleScaleEffector(const Vector2D& startScale, const Vector2D& endScale) :
startScale(startScale), endScale(endScale) {
}

ESPParticleScaleEffector::ESPParticleScaleEffector(float startScale, float endScale) :
effect(), startScale(startScale, startScale), endScale(endScale, endScale) {
}

ESPParticleScaleEffector::ESPParticleScaleEffector(const ScaleEffect& effect) :
effect(effect), startScale(1.0f, 1.0f), endScale(1.0f, 1.0f) {
	this->effect.isInit = true;
}

ESPParticleScaleEffector::~ESPParticleScaleEffector() {
}

void ESPParticleScaleEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	UNUSED_PARAMETER(dT);

	double particleLifeElapsed = particle->GetCurrentLifeElapsed();

	// TODO: move to function...
	// If the effect is initialized then we will affect particles using it
	if (this->effect.isInit) {
		// Pulse effect...
		double amtOfScaling = 1.0;

		// Determine if we are in a growth or shrink cycle...
		double pulseAmt = this->effect.pulseRate * particleLifeElapsed;
		pulseAmt = pulseAmt - floor(pulseAmt);
		assert(pulseAmt >= 0);
		assert(pulseAmt < 1);

		if (pulseAmt < 0.5) {
			// Growth cycle...
			amtOfScaling = 2 * (effect.pulseGrowthScale - 1.0) * pulseAmt + 1.0;
		}
		else if (pulseAmt >= 0.5) {
			// Shrink cycle...
			double twoTimesGS = 2 * this->effect.pulseGrowthScale;
			amtOfScaling = (-twoTimesGS + 2)* pulseAmt + twoTimesGS - 1;
		}
		particle->MultiplyInitSizeScale(Vector2D(amtOfScaling, amtOfScaling));
	}

	// Only do interpolations for particles with discrete lifetimes
	double particleLifeSpan = particle->GetLifespanLength();
	if (particleLifeSpan != ESPParticle::INFINITE_PARTICLE_LIFETIME) {
		// Perform linear interpolation on the scale of the particle
		// based on the particle's lifetime
		Vector2D newScale = this->startScale + particleLifeElapsed * 
			(this->endScale - this->startScale) / particleLifeSpan;

		particle->MultiplyInitSizeScale(newScale);
	}
}