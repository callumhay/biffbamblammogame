#include "ESPParticleColourEffector.h"
#include "ESPParticle.h"

ESPParticleColourEffector::ESPParticleColourEffector(float startAlpha, float endAlpha) :
startAlpha(startAlpha), endAlpha(endAlpha), startColour(1,1,1) {
}

ESPParticleColourEffector::ESPParticleColourEffector(Colour colour, float startAlpha, float endAlpha) :
startAlpha(startAlpha), endAlpha(endAlpha), startColour(colour) {
}

ESPParticleColourEffector::ESPParticleColourEffector(Colour colour, float alpha) :
startAlpha(alpha), endAlpha(alpha), startColour(colour) {
}

ESPParticleColourEffector::~ESPParticleColourEffector() {
}

void ESPParticleColourEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	
	// Only do interpolation when we're dealing with discrete lifetimes
	double particleLifespan = particle->GetLifespanLength();
	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME) {
		// Obtain the current colour and alpha of the particle
		Colour currColour;
		double  currAlpha;
		particle->GetColour(currColour, currAlpha);	
		
		// Perform linear interpolation on the alpha of the particle
		// based on the particle's lifetime
		double newAlpha = this->startAlpha + particle->GetCurrentLifeElapsed() * 
			(this->endAlpha - this->startAlpha) / particleLifespan;

		particle->SetColour(startColour, newAlpha);
	}
}