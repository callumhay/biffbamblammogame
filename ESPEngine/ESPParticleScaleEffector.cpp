/**
 * ESPParticleScaleEffector.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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