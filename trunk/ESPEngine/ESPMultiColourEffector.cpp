/**
 * ESPMultiColourEffector.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#include "ESPMultiColourEffector.h"
#include "ESPParticle.h"

#include "../BlammoEngine/Animation.h"

ESPMultiColourEffector::ESPMultiColourEffector() {
}

void ESPMultiColourEffector::AffectParticleOnTick(double dT, ESPParticle* particle) {
	UNUSED_PARAMETER(dT);

	// If there are colours to interpolate across then we setup an animation for them and
	// tick them to the current point in the particle's life
	if (this->colours.size() == 0) {
		return;
	}
	else if (this->colours.size() == 1) {
		particle->SetColour(this->colours[0]);
		return;
	}

	double particleLifeElapsed = particle->GetCurrentLifeElapsed();
	double totalParticleLife   = particle->GetLifespanLength();
	double percentLifeElapsed	 = particleLifeElapsed / totalParticleLife;

	int lookupIndexBase = static_cast<int>(percentLifeElapsed * static_cast<double>(colours.size()-1));
	int lookupIndexNext = (lookupIndexBase + 1);
	
	if (lookupIndexNext >= static_cast<int>(this->colours.size())) {
		lookupIndexNext = this->colours.size() - 1;
	}

	double startPercentage = static_cast<double>(lookupIndexBase) / static_cast<double>(colours.size()-1);
	double endPercentage	 = static_cast<double>(lookupIndexNext) / static_cast<double>(colours.size()-1);
	ColourRGBA startColour = this->colours[lookupIndexBase];
	ColourRGBA endColour   = this->colours[lookupIndexNext];

	ColourRGBA lerpColour = startColour + (percentLifeElapsed - startPercentage) * (endColour - startColour) / (endPercentage - startPercentage);

	particle->SetColour(lerpColour);

}

void ESPMultiColourEffector::SetColours(const std::vector<ColourRGBA>& colours) {
	this->colours = colours;
	this->percentages.clear();

	if (colours.size() <= 1) {
		return;
	}

	this->percentages.reserve(colours.size());

	double currPercentage = 0.0;
	double percentageIncrement = 1.0 / static_cast<float>(colours.size()-1);
	for (size_t i = 0; i < colours.size(); i++) {
		this->percentages.push_back(currPercentage);
		currPercentage += percentageIncrement;
	}
}