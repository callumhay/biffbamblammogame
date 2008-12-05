#include "ESPParticleColourEffector.h"
#include "ESPParticle.h"

ESPParticleColourEffector::ESPParticleColourEffector(float startAlpha, float endAlpha) :
startAlpha(startAlpha), endAlpha(endAlpha) {
}

ESPParticleColourEffector::~ESPParticleColourEffector() {
}

void ESPParticleColourEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	// Obtain the current colour and alpha of the particle
	Colour currColour;
	float  currAlpha;
	particle->GetColour(currColour, currAlpha);	
	
	// Perform linear interpolation on the alpha of the particle
	// based on the particle's lifetime
	float newAlpha = static_cast<float>(this->startAlpha + particle->GetCurrentLifeElapsed() * 
		(this->endAlpha - this->startAlpha) / particle->GetLifespanLength());

	particle->SetColour(currColour, newAlpha);
}