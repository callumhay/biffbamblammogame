#include "ESPParticleColourEffector.h"
#include "ESPParticle.h"

ESPParticleColourEffector::ESPParticleColourEffector() : 
startAlpha(1), endAlpha(1), startColour(1,1,1), endColour(1,1,1), useStartColour(false) {
}

ESPParticleColourEffector::ESPParticleColourEffector(float startAlpha, float endAlpha) :
startAlpha(startAlpha), endAlpha(endAlpha), startColour(1,1,1), endColour(1,1,1), useStartColour(false) {
}

ESPParticleColourEffector::ESPParticleColourEffector(const Colour& colour, float startAlpha, float endAlpha) :
startAlpha(startAlpha), endAlpha(endAlpha), startColour(colour), endColour(colour), useStartColour(true) {
}

ESPParticleColourEffector::ESPParticleColourEffector(const Colour& colour, float alpha) :
startAlpha(alpha), endAlpha(alpha), startColour(colour), endColour(colour), useStartColour(true) {
}

ESPParticleColourEffector::ESPParticleColourEffector(const ColourRGBA& start, const ColourRGBA& end) :
startAlpha(start.A()), endAlpha(end.A()), startColour(start.R(), start.G(), start.B()), 
endColour(end.R(), end.G(), end.B()), useStartColour(true) {
}

ESPParticleColourEffector::~ESPParticleColourEffector() {
}

void ESPParticleColourEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	
	// Only do interpolation when we're dealing with discrete lifetimes
	double particleLifespan = particle->GetLifespanLength();
	
	// Obtain the current colour and alpha of the particle
	Colour currColour;
	float currAlpha;
	particle->GetColour(currColour, currAlpha);		

	if (particleLifespan != ESPParticle::INFINITE_PARTICLE_LIFETIME) {
		// Perform linear interpolation on the colour and alpha of the particle
		// based on the particle's lifetime
		if (this->useStartColour) {
			currColour = this->startColour + particle->GetCurrentLifeElapsed() * 
			(this->endColour - this->startColour) / particleLifespan;
		}
		
		currAlpha = this->startAlpha + particle->GetCurrentLifeElapsed() * 
			(this->endAlpha - this->startAlpha) / particleLifespan;
	}
	else {
		if (this->useStartColour) {
			currColour = startColour;
		}
	}

	particle->SetColour(currColour, currAlpha);
}
