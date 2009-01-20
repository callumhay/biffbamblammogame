#ifndef __ESPPARTICLECOLOUREFFECTOR_H__
#define __ESPPARTICLECOLOUREFFECTOR_H__

#include "../BlammoEngine/Colour.h"

#include "ESPParticleEffector.h"

class ESPParticleColourEffector : public ESPParticleEffector {
private:
	bool useStartColour;
	float startAlpha, endAlpha;
	Colour startColour;
public:
	ESPParticleColourEffector(float startAlpha, float endAlpha);
	ESPParticleColourEffector(Colour colour, float startAlpha, float endAlpha);
	ESPParticleColourEffector(Colour colour, float alpha);
	~ESPParticleColourEffector();

	virtual void AffectParticleOnTick(double dT, ESPParticle* particle);
};
#endif