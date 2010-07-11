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