/**
 * ESPMultiColourEffector.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPMULTICOLOUREFFECTOR_H__
#define __ESPMULTICOLOUREFFECTOR_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPMultiColourEffector : public ESPParticleEffector {

private:
	std::vector<double> percentages;
	std::vector<ColourRGBA> colours;

public:
	ESPMultiColourEffector();
	~ESPMultiColourEffector() {}
	void AffectParticleOnTick(double dT, ESPParticle* particle);

	void SetColours(const std::vector<ColourRGBA>& colours);
};
#endif